const float HOR_MID_DIST = 3.5;
const float CALIBRATE_BOARD = 9.0;
const int BOARD_ROWS = 6;
const int BOARD_COLS = 7;
const int SLOT_E = 0;
const int SLOT_R = 5;
const int SLOT_Y = 4;
const int BLUE = 2;
const float COL_1 = 11.58;
const float COL_2 = 15.38;
const float COL_3 = 19.08;
const float COL_4 = 22.85;
const float COL_5 = 26.82;
const float COL_6 = 30.22;
const float COL_7 = 33.92;

int gameBoard[BOARD_ROWS][BOARD_COLS];

void Dispense() //Dispenses a chip
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
	motor[motorB]= -100;
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
	for (int i = col - 1; i >=0 && cont == true; i++)
	{
		if (gameBoard[row][i] == token)
			counter++;
		else
			cont = false;
	}
	return counter > 3;
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
	for (int i = row - 1; i >=0 && cont == true; i++)
	{
		if (gameBoard[i][col] == token)
			counter++;
		else
			cont = false;
	}
	return counter > 3;
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
	if (counter > 3)
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
	return counter > 3;
}

void test(float *columns ) //Test hardware
{
	for(int i = 0; i < 7; i++)
	{
		recallHorSensors();
		wait1Msec(500);
		motor[motorA]= 25;
		while (nMotorEncoder[motorA] < ((180/(PI*2.1575))*(columns[i]))){}
		motor[motorA] = 0;
		wait1Msec(50);
		Dispense();
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
	float columns[7] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7};

	//creates virtual gameboard and fills it with all empty values

	while(!getButtonPress(buttonAny)){}
	test(columns);
	recallHorSensors();

	//added some stuff

}
