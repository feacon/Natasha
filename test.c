//const float CALIBRATE_BOARD = 9.0;
const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.82;
const float COL_6 = 30.7;
const float COL_7 = 34.9;
const float CENT_CENT = 3.42;

int gameBoard[BOARD_ROWS][BOARD_COLS];
int movePath[4][2];
int bestMovePath[4];

void dispense() //Dispenses a chip
{
	motor[motorC] = 20;
	wait1Msec(350);
	motor[motorC] = -20;
	wait1Msec(1000);
	motor[motorC] = 0;
	//while(getButtonPress(buttonAny)){}
}

void recallVerSensors() //resets the motor encoder for the motor running in the vertical xis
{
	motor[motorB]= -40;
	while(SensorValue[S1] == 0){}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
}

void recallHorSensors() //resets the motor encoder for the motor running in the horizontal axis
{
	motor[motorA]= -35;
	while(SensorValue[S2] == 0){}
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
}

int checkBoard()
{
	recallVerSensors();
	recallHorSensors();
	nMotorEncoder[motorA]= 0;
	motor[motorA] = 25;
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*COL_7)){
		if (SensorValue[S3] == (int)colorBlue)
		{
			recallHorSensors();
			return 1;
		}
	}
	return -1;

}

bool rowCheck(int row, int col, int token)
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;
	bool cont = true;

	//go right
	for (int i = col + 1; i < BOARD_COLS && cont == true; i++)
	{
		if (gameBoard[row][i] == token)
			counter++;
		else
			cont = false;
	}
	cont = true;
	//go left
	for (int i = col - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[row][i] == token)
			counter++;
		else
			cont = false;
	}
	return counter >= 4;
}

bool colCheck (int row, int col, int token)
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;
	bool cont  = true;
	//go down
	for (int i = row + 1; i < BOARD_ROWS && cont == true; i++)
	{
		if (gameBoard[i][col] == token)
			counter++;
		else
			cont = false;
	}
	cont = true;
	//go up
	for (int i = row - 1; i > 0 && cont == true; i--)
	{
		if (gameBoard[i][col] == token)
			counter++;
		else
			cont = false;
	}
	return counter >= 4;
}

bool diagonalCheck(int row, int col, int token)
{
	if (gameBoard[row][col] != token)
		return false;

	int counter = 1;

	//bottom left to top right
	int trow = row - 1, tcol = col + 1;
	bool cont = true;
	while (trow >= 0 && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow--;
		tcol++;
	}

	trow = row + 1;
	tcol = col - 1;
	cont = true;
	//top right to bottom left
	while (trow < BOARD_ROWS && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow++;
		tcol--;
	}
	if (counter >= 4)
		return true;

	//top left to bottom right
	counter = 1;
	trow = row + 1;
	tcol = col + 1;
	cont = true;
	while (trow < BOARD_ROWS && tcol < BOARD_COLS && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow++;
		tcol++;
	}

	//bottom right to top left
	trow = row - 1;
	tcol = col - 1;
	cont = true;
	while (trow >= 0 && tcol >= 0 && cont == true)
	{
		if (gameBoard[trow][tcol] == token)
			counter++;
		else
			cont = false;
		trow--;
		tcol--;
	}
	return counter >= 4;
}

void test(float *columns, int color) //Test hardware
{
	for(int i = 0; i < 7; i++)
	{
		recallHorSensors();
		wait1Msec(500);
		motor[motorA]= 25;
		while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[random(6)]))){}
		motor[motorA] = 0;
		displayBigTextLine(3, "%d ",color);
		wait1Msec(500);
		eraseDisplay();
		wait1Msec(50);
		dispense();
	}
}

int scanCol(int colNum)
{
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		wait1Msec(50);
		if (SensorValue[S3] == (int)colorYellow)
		{
			gameBoard[i][colNum] = (int)colorYellow;
			return i;
		}
		else if(i < (BOARD_ROWS-1))//checks to see if board already has next index saved
		{
			if (gameBoard[i+1][colNum] != 0)
				return -1; // dummy place hold value to exit loop
		}
		if (i<BOARD_ROWS-1)
		{
			nMotorEncoder[motorB] = 0;
			motor[motorB] = 15;
			while (nMotorEncoder[motorB] < ((180/(PI*1.1))*(CENT_CENT))){}
			motor[motorB] = 0;
		}
	}
	recallVerSensors();
	return -1;
}

int dropPiece (int col, int token)
{
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		if (i != 5)
		{
			if (gameBoard[i+1][col] != 0)
			{
				return i;
			}
		}
		else if(gameBoard[5][col] == 0)
		{
			return 5;
		}
	}
	return 0;
}

void displayWin(int condition)
{
	eraseDisplay();
	if (condition == 1)
		displayBigTextLine(3, "PLAYER WINS");
	else if (condition == 2)
		displayBigTextLine(3, "gg ez :^)");
	else
		displayBigTextLine(3, "Tie!");
}

int checkWin(int row, int col, int token)
{
	if (rowCheck(row, col, token) || colCheck(row, col, token) ||
		diagonalCheck(row, col, token))
	{
		if (token == 5)
			return 2;
		else if (token == 4)
			return 1;

	}
	else
	{
		for(int i = 0; i < BOARD_ROWS; i++)
		{
			for(int j = 0; j < BOARD_COLS; j++)
			{
				if (gameBoard[i][j] == 0)
					return 0;
			}
		}
		return 3;
	}
	return 0;
}



int makeDecision()
{
	int rowNum = 0, currentSum = 0, bestSum = 0;

	for (int i = 0; i < 4; i++)
	{
		movePath[i][0] = 0;
		movePath[i][1] = 0;
		bestMovePath[i] = 0;
	}
	for (int i = 0; i < BOARD_COLS; i++)
	{

		rowNum = dropPiece(i, (int)colorRed);
		movePath[0][0]=i;
		if (checkWin(rowNum, i, (int)colorRed))
		{
			movePath[0][1] = 1;
		}
		for(int j = 0; j < BOARD_COLS; j++)
		{
			rowNum = dropPiece(j, (int)colorYellow);
			movePath[1][0]=j;
			if (checkWin(rowNum, j, (int)colorYellow))
			{
				movePath[1][1] = -1;
			}
			for(int k = 0; k < BOARD_COLS; k++)
			{
				rowNum = dropPiece(k, (int)colorRed);
				movePath[2][0]=k;
				if (checkWin(rowNum, k, (int)colorRed))
				{
					movePath[2][1] = 1;
				}
				for(int l = 0; l < BOARD_COLS; l++)
				{
					rowNum = dropPiece(l, (int)colorYellow);
					movePath[3][0]=l;
					if (checkWin(rowNum, l, (int)colorYellow))
					{
						movePath[3][1] = -1;
						currentSum = movePath[0][1] + movePath[1][1] + movePath[2][1] + movePath[3][1];
						if (currentSum > bestSum)
						{
							bestMovePath[0] = movePath[0][0];
							bestMovePath[1] = movePath[1][0];
							bestMovePath[2] = movePath[2][0];
							bestMovePath[3] = movePath[3][0];
							bestSum = currentSum;
						}
						for (int i = 0; i < 4; i++)
						{
							movePath[i][0] = 0;
							movePath[i][1] = 0;
						}

					}
				}
			}
		}
	}
	return bestMovePath[0];
}

void driveAndDispense(float *columns, int &row, int &col) //Test hardware
{
	wait1Msec(500);
	motor[motorA]= 25;
	col = random(6);
	//col = makeDecision();
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[col]))){}
	motor[motorA] = 0;
	wait1Msec(500);
	eraseDisplay();
	wait1Msec(50);
	dispense();
	for(int i = 0; i < BOARD_ROWS; i++)
	{
		if (i != 5)
		{
			if (gameBoard[i+1][col] != 0)
			{
				gameBoard[i][col] = (int)colorRed;
				row = i;
			}
		}
		else if(gameBoard[5][col] == 0)
		{
			gameBoard[5][col] = (int)colorRed;
			row = 5;
		}
	}
}
task main()
{
	SensorType[S1] = sensorEV3_Touch;
	SensorType[S2] = sensorEV3_Touch;
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S4] = sensorEV3_Touch;
	float columns[7] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7};



	/*for(int i = 0; i < BOARD_COLS; i++)
	{
	gameBoard[5][i] = (int)colorRed;
	}

	recallVerSensors();
	recallHorSensors();
	for(int i = 0; i < BOARD_COLS; i++)
	{
	wait1Msec(500);
	motor[motorA]= 20;
	while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
	motor[motorA] = 0;
	scanCol(i);
	recallVerSensors();
	}*/

	//test(columns, SensorValue[S3]);
	if (checkBoard() == 1)
	{
		int win = 0;
		bool playAgain = true;//0= no win, 1 = player win, 2 = robot win, 3 = tie
		while(playAgain == true)
		{
			for(int i = 0; i < BOARD_ROWS; i++)
			{
				for(int j = 0; j < BOARD_COLS; j++)
				{
					gameBoard[i][j] = 0;
				}
			}
			while(win == 0)
			{
				//wait for player turn
				for(int i = 0;i < BOARD_ROWS; i++)
				{
					displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],gameBoard[i][6]);
				}
				while(!SensorValue[S4]){}
				eraseDisplay();
				int lastPieceRow = 0, lastPieceCol = 0;//change this to touch sensor when we get it
				for(int i = 0; i < BOARD_COLS; i++)
				{
					motor[motorA]= 20;
					while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
					motor[motorA] = 0;
					lastPieceRow = scanCol(i);
					lastPieceCol = i;
					if (lastPieceRow != -1)
						i = 7;
					recallVerSensors();
				}
				recallHorSensors();

				if (checkWin(lastPieceRow, lastPieceCol, (int)colorYellow) == 1)
				{
					win = 1;
					eraseDisplay();
					displayBigTextLine(3, "PLAYER WINS");
					wait1Msec(5000);
				}
				else
				{
					driveAndDispense(columns, lastPieceRow, lastPieceCol);
					if (checkWin(lastPieceRow, lastPieceCol, (int)colorRed) == 2)
					{
						win = 2;
						eraseDisplay();
						displayBigTextLine(3, "NATASHA WINS");
						wait1Msec(5000);
					}
					else
					{
						recallVerSensors();
						recallHorSensors();
					}
				}
			}
			eraseDisplay();
			displayBigTextLine(5, "PLAY AGAIN?");
			displayBigTextLine(8, "DOWN FOR NO");
			displayBigTextLine(10, "ANY FOR YES");
			wait1Msec(5000);
			eraseDisplay();
			for(int i = 0;i < BOARD_ROWS; i++)
			{
				displayBigTextLine(2 * i, "%d %d %d %d %d %d %d", gameBoard[i][0],gameBoard[i][1],gameBoard[i][2],gameBoard[i][3],gameBoard[i][4],gameBoard[i][5],gameBoard[i][6]);
			}
			win = 0;
			while(!getButtonPress(buttonAny)){}

			while(SensorValue[S4] == 1)
			{
				eraseDisplay();
				playAgain = false;
			}
			eraseDisplay();
		}
	}
}
