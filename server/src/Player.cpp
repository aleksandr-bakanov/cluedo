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
			case C_GUEST_MOVE: makeMoveHandler(); break;
			case C_ASK: askHandler(); break;
			case C_ANSWER: answerHandler(); break;
			case C_GUESS_SECRET: guessSecretHandler(); break;
			case C_NO_CARDS: noCardsHandler(); break;
			case C_END_TURN: endTurnHandler(); break;
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
	leaveRoomHandler();
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
	myTurn = inGame = mustAsk = isLose = false;
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
	short cmdSize = 3;
	short cmdId = S_AVAILABLE_GUESTS;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guestMap;
	sendData(5);
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
Player::sendStartInfo(const char * order)
{
	short cmdId = S_START_GAME_INFO;
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guest;
	sendBuf[5] = x;
	sendBuf[6] = y;
	int i;
	short cmdSize = 5;
	for (i = 0; i < 6; i++)
		sendBuf[++cmdSize + 1] = order[i] > 10 ? order[i] - 10 : order[i];
	for (i = 0; cards[i]; i++)
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
	sendBuf[4] = guestId > 10 ? guestId - 10 : guestId;
	sendBuf[5] = firstDie;
	sendBuf[6] = secondDie;
	sendBuf[7] = ONE_TURN_DELAY;
	sendData(8);
}

void
Player::makeMoveHandler()
{
	if (inGame && myTurn && steps && !mustAsk && !isLose)
	{
		char x = BCgetChar(recvBuf, curRecvPos);
		char y = BCgetChar(recvBuf, curRecvPos);
		Room * r = (Room *)room;
		r->guestMakeMove((void *)this, x, y);
	}
}

void
Player::sendGuestMakeMove(char guestId, vector<char> &v)
{
	short cmdSize = SHORT_SIZE + 1 + v.size();
	short cmdId = S_GUEST_MOVE;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guestId;
	int len = v.size();
	for (int i = 0, j = len - 1; i < len; i++, j--)
		sendBuf[i + 5] = v[j];
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::sendTransGuest(char guestId, char x, char y)
{
	short cmdSize = SHORT_SIZE + 3;
	short cmdId = S_TRANS_GUEST;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guestId;
	sendBuf[5] = x;
	sendBuf[6] = y;
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::askHandler()
{
	if (inGame && myTurn && app && app != lastAskedApp && !isLose)
	{
		char gt = BCgetChar(recvBuf, curRecvPos);
		char wp = BCgetChar(recvBuf, curRecvPos);
		if (gt >= 0 && gt < 7 && wp >= 7 && wp < 16)
			((Room *)room)->playerAsk(this, gt, wp);
	}
}

void
Player::sendPlayerAsk(char enquirer, char ap, char gt, char wp)
{
	short cmdSize = 6;
	short cmdId = S_PLAYER_ASK;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = enquirer > 10 ? enquirer - 10 : enquirer;
	sendBuf[5] = ap;
	sendBuf[6] = gt > 10 ? gt - 10 : gt;
	sendBuf[7] = wp;
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::sendPlayerAnswer(char gt, char card)
{
	short cmdSize = 3 + (card ? 1 : 0);
	short cmdId = S_PLAYER_ANSWER;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = gt > 10 ? gt - 10 : gt;
	if (card)
		sendBuf[5] = card;
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::sendWaitAnswer(char gt, char seconds)
{
	short cmdSize = 4;
	short cmdId = S_WAIT_ANSWER;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = gt > 10 ? gt - 10 : gt;
	sendBuf[5] = seconds;
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::sendNoCards(char guestId, char ap, char gt, char wp)
{
	short cmdSize = 6;
	short cmdId = S_NO_CARDS;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guestId > 10 ? guestId - 10 : guestId;
	sendBuf[5] = ap;
	sendBuf[6] = gt;
	sendBuf[7] = wp;
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::answerHandler()
{
	if (inGame)
	{
		char card = BCgetChar(recvBuf, curRecvPos);
		Room * r = (Room *)room;
		r->playerAnswer((void *)this, card);
	}
}

void
Player::guessSecretHandler()
{
	if (inGame && myTurn && !isLose)
	{
		char ap = BCgetChar(recvBuf, curRecvPos);
		char gt = BCgetChar(recvBuf, curRecvPos);
		char wp = BCgetChar(recvBuf, curRecvPos);
		Room * r = (Room *)room;
		cout << "  ap = " << int(ap) << "  gt = " << int(gt) << "  wp = " << int(wp) << endl;
		r->playerGuessSecret((void *)this, ap, gt, wp);
	}
}

void
Player::sendGuessSecret(char guestId, char ap, char gt, char wp)
{
	short cmdSize = ap ? 6 : 3;
	short cmdId = S_GUESS_SECRET;
	memcpy(sendBuf, &cmdSize, SHORT_SIZE);
	memcpy(sendBuf + 2, &cmdId, SHORT_SIZE);
	sendBuf[4] = guestId > 10 ? guestId - 10 : guestId;
	if (ap)
	{
		sendBuf[5] = ap;
		sendBuf[6] = gt;
		sendBuf[7] = wp;
	}
	sendData(cmdSize + SHORT_SIZE);
}

void
Player::noCardsHandler()
{
	if (inGame && !isLose)
	{
		Room * r = (Room *)room;
		r->playerHasNoCards((void *)this);
	}
}

void
Player::endTurnHandler()
{
	if (inGame && myTurn && !isLose)
	{
		Room * r = (Room *)room;
		r->playerEndsTurn((void *)this);
	}
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
		if (open.size() == 0)
			break;
	}
	v.clear();
	if (isPathFound)
	{
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
