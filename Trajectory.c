#pragma config(Motor,  motorA,          InMotor,       tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          LeftMotor,     tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          RightMotor,    tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define R 2.76
#define L 12.3
#define F 7.75

#define PI 3.1415926536

typedef struct coord {
	float x;
	float y;
} point;

//Global variables - you will need to change some of these
float robot_X = 0.0, robot_Y = 0.0, robot_TH = 0.0;
float marker_X, marker_Y, marker_vX, marker_vY, marker_w;
int oldL = 0, oldR = 0, oldT = 0;
int velocityUpdateInterval = 10;
int PIDUpdateInterval = 15;

float max(float a, float b){
	if(a > b)
		return a;
	return b;
}

/*float min(float a, float b){
	if(a < b)
		return a;
	return b;
}*/

/*****************************************
 * Complete this function so that it
 * continuously updates the robot's position
 *****************************************/
task dead_reckoning()
{
	oldL = nMotorEncoder[LeftMotor];
	oldR = nMotorEncoder[RightMotor];
	oldT = nPgmTime;
	int loops = 0;
	while(1)
	{
		int curTime = nPgmTime;
		int leftEnc = nMotorEncoder[LeftMotor];
		int rightEnc = nMotorEncoder[RightMotor];
		float t = (curTime - oldT) / 1000.0;
		if(t == 0){
			continue;
		}
		loops++;
		float vl = (leftEnc - oldL) / t * PI / 180.0 * R;
		float vr = (rightEnc - oldR) / t * PI / 180.0 * R;

		//float v2 = min(vr, vl);
		float vmax = max(abs(vr), abs(vl));
		float r1;
		if(vr != vl){
			r1 = abs(vmax*L/(vr - vl));
		}
		else{
			r1 = L/2;
		}

		float v = (vr + vl) / 2.0;

		if(v < 0){
			r1 = r1 * -.000455 + 1.0045;
		}
		else{
			r1 = r1 * -.002191 + .9885124;
		}
		float w = (vr - vl) / L;// * r1;

		float k00 = v*cos(robot_TH);
		float k01 = v*sin(robot_TH);
		float k02 = w;
		float k10 = v*cos(robot_TH + k02*.5*t);
		float k11 = v*sin(robot_TH + k02*.5*t);
		float k12 = w;
		float k20 = v*cos(robot_TH + k12*.5*t);
		float k21 = v*sin(robot_TH + k12*.5*t);
		float k22 = w;
		float k30 = v*cos(robot_TH + k22*t);
		float k31 = v*sin(robot_TH + k22*t);
		float k32 = w;

		robot_X += t/6.0 * (k00 + 2*(k10 + k20) + k30);
		robot_Y += t/6.0 * (k01 + 2*(k11 + k21) + k31);
		robot_TH -= t/6.0 * (k02 + 2*(k12 + k22) + k32);

		marker_X = robot_X + F * cos(robot_TH);
		marker_Y = robot_Y + F * sin(robot_TH);
		//marker_vX =

		//Code that plots the robot's current position and also prints it out as text
		setPixel(50 + (int)(100.0 * robot_X), 32 + (int)(100.0 * robot_Y));
		displayTextLine(0, "X: %f", robot_X);
		displayTextLine(1, "Y: %f", robot_Y);
		displayTextLine(2, "t: %f", 57.2958 * robot_TH);

		displayTextLine(6, "mX: %f", marker_X);
		displayTextLine(7, "mY: %f", marker_Y);


		wait1Msec(velocityUpdateInterval);
		oldL = leftEnc;
		oldR = rightEnc;
		oldT = curTime;
	}
}

/*****************************************
 * Trajectories - these functions define
 * the sample trajectories
 *****************************************/

float clamp (float x, float minimum, float maximum)
{
	if (x < minimum)
		return minimum;
	else if (x > maximum)
		return maximum;
	else
		return x;
}

void traj1 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	p->x = 0.5 * cos(t/10) * sin(t/10);
	p->y = 0.2 * sin(t/10) * sin(t/5);
}

void traj2 (float t, point *p)
{
	t = clamp(t, 0.0, PI*10);
	p->x = 0.2 * sin(3*t/5);
	p->y = 0.2 * cos(2*(t/5 + PI/4));
}

void traj3 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	p->x = 0.2 * cos(t/10) * cos(t/5);
	p->y = 0.2 * cos(3*t/10) * sin(t/10);
}

void traj4 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	p->x = 0.2 * (0.5 * cos(3*t/10) - 0.75 * cos(t/5));
	p->y = 0.2 * (-0.75 * sin(t/5) - 0.5 * sin(3*t/10));
}

void traj5 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	float c = cos(t/5);
	p->x = 0.1 * (-2*c*c - sin(t/10) + 1) * sin(t/5);
	p->y = 0.1 * c *(-2*c*c*c - sin(t/10) + 1);
}

void traj6 (float t, point *p)
{
	t = clamp(t, 0.0, PI*24);
	float c = cos(t/12);
	float s = sin(t/4);
	p->x = 0.1 * (2*c*c*c + 1)*s;
	p->y = 0.1 * cos(t/4) * (1 - 2*s*s*s*s);
}

void traj7 (float t, point *p)
{
	t = clamp(t, 0.0, PI*40);
	p->x = 0.04 * (5*cos(9*t/20) - 4*cos(t/4));
	p->y = 0.04 * (-4*sin(t/4) - 5*sin(9*t/20));
}

void trajectory (int traj, float t, point *p)
{
	switch (traj) {
		case 0:
			traj1(t, p);
			break;
		case 1:
			traj2(t, p);
			break;
		case 2:
			traj3(t, p);
			break;
		case 3:
			traj4(t, p);
			break;
		case 4:
			traj5(t, p);
			break;
		case 5:
			traj6(t, p);
			break;
		case 6:
			traj7(t, p);
			break;
	}
}

/*****************************************
 * get_trajectory - determine trajectory
 * to follow
 *****************************************/

 int get_trajectory ()
 {
   int traj = 0;
   while (nNxtButtonPressed != kEnterButton)
   {
     displayTextLine(0, "Trajectory %d", (traj+1));
     if (nNxtButtonPressed == kLeftButton)
       traj--;
     else if (nNxtButtonPressed == kRightButton)
       traj++;

     if (traj < 0)
       traj = 6;
     else if (traj > 6)
       traj = 0;

     wait1Msec(150);
   }
   return traj;
 }

/*****************************************
 * Main function - it is not necessary to
 * modify this
 *****************************************/
task main()
{
  /* Reset encoders and turn on PID control */
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	nMotorPIDSpeedCtrl[motorB] = mtrSpeedReg;
	nMotorPIDSpeedCtrl[motorC] = mtrSpeedReg;
	nPidUpdateInterval = PIDUpdateInterval;

  motor[motorB] = 0;
	motor[motorC] = 0;
	nNxtButtonTask  = 0;

	startTask(dead_reckoning);
	for (int i=0; i < 3; i++) {
		motor[motorB] = 10 + abs(rand() % 40);
		motor[motorC] = 10 + abs(rand() % 40);
		wait1Msec(1000);
	}

	motor[motorB] = 0;
	motor[motorC] = 0;

	while (nNxtButtonPressed != kEnterButton) {}
	stopTask(dead_reckoning);

	// draw the graph

	int traj = get_trajectory();
	point p;

	float t = 0.0;
	float old_time = nPgmTime;
	float curTime;

	while(nNxtButtonPressed != kExitButton) {
		curTime = nPgmTime;
		t += (curTime - old_time) / 1000.0;
		trajectory(traj, t, &p);

		float x = p.x;
		float y = p.y;

		setPixel(50 + (int)(100.0 * x), 32 + (int)(100.0 * y));

		displayTextLine(0, "%0.2f, %0.2f", x, y);
		displayTextLine(7, "t: %0.2f", t);

		old_time = curTime;

		wait1Msec(velocityUpdateInterval);
	}
}
