#include "include/Player.hpp"
#include "include/BytesConverter.hpp"
#include "include/RoomManager.hpp"
#include "include/Room.hpp"

Player::Player(int socketDescriptor)
{
    socket = socketDescriptor;
    curRecvPos = 0;
    memset(recvBuf, 0, RECV_BUF_SIZE);
    memset(sendBuf, 0, RECV_BUF_SIZE);
    room = NULL;
    resetGameInfo();
    cout << "Player::Player(" << socket << ")" << endl;
}

Player::~Player()
{
    leaveRoomHandler();
    close(socket);
    cout << "Player::~Player(" << socket << ")" << endl;
}

void
Player::run()
{
    sendReadyCommand();
    while (true)
    {
        int recvCount = 0;
        recvCount = read(socket, recvBuf, RECV_BUF_SIZE - curRecvPos);
        if (recvCount <= 0)
            break;
        else
        {
            curRecvPos += recvCount;
            parse();
        }
    }
}

void 
Player::parse()
{
    short comSize = BCgetShort(recvBuf);
    if (curRecvPos - SHORT_SIZE >= comSize)
    {
        curRecvPos = SHORT_SIZE;
        short comId = BCgetShort(recvBuf, curRecvPos);
        switch (comId)
        {
            case C_ENTER_ROOM: enterRoomHandler(); break;
            case C_LEAVE_ROOM: leaveRoomHandler(); break;
            case C_CHOOSE_GUEST: chooseGuestHandler(); break;
            case C_MAKE_STEP: makeStepHandler(); break;
            default: break;
        }
        curRecvPos = 0;
    }
}

bool 
Player::sendData(int nbytes)
{
    if (nbytes <= 0) 
        return false;
    int sendCount = write(socket, sendBuf, nbytes);
    if (sendCount <= 0)
        return false;
    else
        return true;
}

void 
Player::sendReadyCommand()
{
    short cmdSize = SHORT_SIZE;
    short cmdId = S_READY;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendData(SHORT_SIZE * 2);
}

void 
Player::enterRoomHandler()
{
    char type = BCgetChar(recvBuf, curRecvPos);
    if (type >= 3 && type <= 6)
    {
        RoomManager * rm = (RoomManager *)roomManager;
        bool success = rm->addPlayerIntoRoom((void *)this, type);
        if (!success)
            sendNoRoom();
    }
}

void
Player::sendNoRoom()
{
    short cmdSize = SHORT_SIZE;
    short cmdId = S_NO_ROOM;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendData(SHORT_SIZE * 2);
}

void 
Player::resetGameInfo()
{
    guest = x = y = app = lastAskedApp = steps = 0;
    for (int i = 0; i < MAX_CARDS; i++)
        cards[i] = 0;
    myTurn = inGame = mustAsk = false;
}

void 
Player::leaveRoomHandler()
{
    if (room)
    {
        Room * r = (Room *)room;
        r->removePlayer((void *)this);
    }
}

void
Player::sendAvailableGuests(char guestMap)
{
    short cmdSize;
    int pos = 4;
    short cmdId = S_AVAILABLE_GUESTS;
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    for (int i = 0; i < MAX_GUESTS; i++)
        if (guestMap & char(pow(2, i)))
            sendBuf[pos++] = char(i + 1);
    cmdSize = SHORT_SIZE + pos - 4;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    sendData(pos);
}

void 
Player::chooseGuestHandler()
{
    if (room && !guest)
    {
        char id = BCgetChar(recvBuf, curRecvPos);
        if (id >= 1 && id <= MAX_GUESTS)
        {
            Room * r = (Room *)room;
            bool result = r->chooseGuest(id);
            if (result)
                guest = id;
            sendGuestChooseResult(id, result);
        }
    }
}

void 
Player::sendGuestChooseResult(char guestId, bool result)
{
    short cmdSize = 3;
    short cmdId = S_GUEST_CHOOSE_RESULT;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    unsigned char res = 0;
    res |= char(pow(2, guestId - 1));
    if (result) res |= char(64);
    sendBuf[4] = res;
    sendData(5);
}

char
Player::addCard(char card)
{
    for (char i = 0; i < MAX_CARDS; i++)
        if (!cards[i])
        {
            cards[i] = card;
            return i;
        }
    return char(-1);
}

void
Player::sendStartInfo()
{
    short cmdId = S_START_GAME_INFO;
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendBuf[4] = guest;
    sendBuf[5] = x;
    sendBuf[6] = y;
    short cmdSize = 5;
    for (int i = 0; cards[i]; i++)
        sendBuf[++cmdSize + 1] = cards[i];
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    sendData(cmdSize + SHORT_SIZE);
}

void
Player::sendNextMove(char guestId, char firstDie, char secondDie)
{
    short cmdSize = 5;
    short cmdId = S_NEXT_MOVE;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendBuf[4] = guestId;
    sendBuf[5] = firstDie;
    sendBuf[6] = secondDie;
    sendData(7);
}

void
Player::makeStepHandler()
{
    if (inGame && myTurn && steps && !mustAsk)
    {
        char x = BCgetChar(recvBuf, curRecvPos);
        char y = BCgetChar(recvBuf, curRecvPos);
        Room * r = (Room *)room;
        r->guestMakeStep((void *)this, x, y);
    }
}

void
Player::sendGuestMakeStep(char guestId, char x, char y)
{
    short cmdSize = 5;
    short cmdId = S_NEXT_MOVE;
    memcpy(sendBuf, &cmdSize, SHORT_SIZE);
    memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
    sendBuf[4] = guestId;
    sendBuf[5] = x;
    sendBuf[6] = y;
    sendData(7);
}

void
Player::aStar(char x, char y, char tox, char toy, vector<char> &v)
{
    // Create map copy
    char map[25][24];
    char i, j;
    for (i = 0; i < 25; i++)
        for (j = 0; j < 24; j++)
            map[i][j] = Room::map[i][j];
    char len = (char) v.size();
    // Add impassible cells.
    for (i = 0; i < len; i++)
        map[v[i + 1]][v[i]] = '#';
    // Creates open and close lists
    vector<Cell> open;
    vector<Cell> close;
    open.clear();
    close.clear();
    // Add start cell to open list
    bool isPathFound = false;
    // cc = currentCell
    Cell cc;
    cc.x = x;
    cc.y = y;
    cc.g = 0;
    cc.h = cc.f = abs(x - tox) + abs(y - toy);
    open.push_back(cc);
    while (!isPathFound)
    {
        int minFCellIndex = minFCell(open);
        // cc = currentCell
        cc = open[minFCellIndex];
        close.push_back(cc);
        open.erase(open.begin() + minFCellIndex);
        // ac = addedCell
        Cell ac;
        int acIndex;
        // To left
        if (cc.x - 1 >= 0 && map[cc.y][cc.x - 1] != '#' &&
            exist(cc.x - 1, cc.y, close) < 0)
        {
            if ((acIndex = exist(cc.x - 1, cc.y, open)) < 0)
            {
                ac.x = cc.x - 1; ac.y = cc.y;
                ac.px = cc.x; ac.py = cc.y;
                ac.g = cc.g + 10;
                ac.h = abs(ac.x - tox) + abs(ac.y - toy);
                ac.f = ac.g + ac.h;
                open.push_back(ac);
            }
            else
            {
                ac = open[acIndex];
                if (ac.g > (cc.g + 10))
                {
                    ac.px = cc.x; ac.py = cc.y;
                    ac.g = cc.g + 10; ac.f = ac.g + ac.h;
                }
            }
        }
        // To right
        if (cc.x + 1 < 24 && map[cc.y][cc.x + 1] != '#' &&
            exist(cc.x + 1, cc.y, close) < 0)
        {
            if ((acIndex = exist(cc.x + 1, cc.y, open)) < 0)
            {
                ac.x = cc.x + 1; ac.y = cc.y;
                ac.px = cc.x; ac.py = cc.y;
                ac.g = cc.g + 10;
                ac.h = abs(ac.x - tox) + abs(ac.y - toy);
                ac.f = ac.g + ac.h;
                open.push_back(ac);
            }
            else
            {
                ac = open[acIndex];
                if (ac.g > (cc.g + 10))
                {
                    ac.px = cc.x; ac.py = cc.y;
                    ac.g = cc.g + 10; ac.f = ac.g + ac.h;
                }
            }
        }
        // To up
        if (cc.y - 1 >= 0 && map[cc.y - 1][cc.x] != '#' &&
            exist(cc.x, cc.y - 1, close) < 0)
        {
            if ((acIndex = exist(cc.x, cc.y - 1, open)) < 0)
            {
                ac.x = cc.x; ac.y = cc.y - 1;
                ac.px = cc.x; ac.py = cc.y;
                ac.g = cc.g + 10;
                ac.h = abs(ac.x - tox) + abs(ac.y - toy);
                ac.f = ac.g + ac.h;
                open.push_back(ac);
            }
            else
            {
                ac = open[acIndex];
                if (ac.g > (cc.g + 10))
                {
                    ac.px = cc.x; ac.py = cc.y;
                    ac.g = cc.g + 10; ac.f = ac.g + ac.h;
                }
            }
        }
        // To down
        if (cc.y + 1 < 25 && map[cc.y + 1][cc.x] != '#' &&
            exist(cc.x, cc.y + 1, close) < 0)
        {
            if ((acIndex = exist(cc.x, cc.y + 1, open)) < 0)
            {
                ac.x = cc.x; ac.y = cc.y + 1;
                ac.px = cc.x; ac.py = cc.y;
                ac.g = cc.g + 10;
                ac.h = abs(ac.x - tox) + abs(ac.y - toy);
                ac.f = ac.g + ac.h;
                open.push_back(ac);
            }
            else
            {
                ac = open[acIndex];
                if (ac.g > (cc.g + 10))
                {
                    ac.px = cc.x; ac.py = cc.y;
                    ac.g = cc.g + 10; ac.f = ac.g + ac.h;
                }
            }
        }
        if (exist(tox, toy, open) >= 0)
            isPathFound = true;
    }
    v.clear();
    char rx = tox;
    char ry = toy;
    int ind;
    while (!(rx == x && ry == y))
    {
        v.push_back(ry);
        v.push_back(rx);
        if ((ind = exist(rx, ry, open)) >= 0)
            cc = open[ind];
        else
            cc = close[exist(rx, ry, close)];
        rx = cc.px;
        ry = cc.py;
    }
}

int
Player::exist(char x, char y, vector<Cell> &v)
{
    int len = v.size();
    for (int i = 0; i < len; i++)
        if (x == v[i].x && y == v[i].y)
            return i;
    return -1;
}

int
Player::minFCell(vector<Cell> &v)
{
    int len = v.size();
    int r = 0;
    char curF = 100;
    char nextF;
    for (int i = 0; i < len; i++) {
        nextF = v[i].f;
        if (nextF < curF) {
            curF = nextF;
            r = i;
        }
    }
    return r;
}
