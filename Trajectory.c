#pragma config(Motor,  motorA,          InMotor,       tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          LeftMotor,     tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          RightMotor,    tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define R 2.76
#define L 12.7
#define F 7.3

#define KP 15.0

#define PI 3.1415926536
#define numPoints 73

typedef struct coord {
	float x;
	float y;
} point;

void trajectory (int traj, float t, point *p);

//Global variables - you will need to change some of these
float robot_TH = 0.0;
int tn;
point robot_c, marker_c, marker_d;
float marker_vd, marker_wd;
int oldL = 0, oldR = 0, oldT = 0;
int velocityUpdateInterval = 20;
int PIDUpdateInterval = 2;

float points[numPoints][2] = {{3.5, 10.2}, {5.0,6.8}, {8.8,5.8}, {11.7,7.2}, {12.7,10.7}, {11.0, 13.9}, {7.0,17.7}, {4.3,20.2},
		{3.5,22.5}, {6.6,22.3}, {9.5,22.3},{12.2,22.3}, {13.8,21.3}, {13.5,19.4}, {14.8,18.6}, {15.4,14.6}, {16.7,12.5}, {17.9,11.2},
		{20.8,10.6}, {20.8,10.0}, {19.8,9.3},{19.7,5.3}, {20.7,4.7}, {21.5,4.4}, {21.5,3.1}, {25.1,3.1}, {25.1,4.4}, {27.2,4.7}, {26.8,5.9},
		{27.5,9.5}, {26.1,9.9}, {26.1,10.7},{28.7,11.4}, {30.9,13.4}, {32.0,16.1}, {32.5,17.9}, {32.0,18.5}, {33.2,19.5}, {33.1,21.3},
		{32.1,20.0}, {31.0,19.6}, {30.5,20.5},{30.7,21.7}, {29.5,20.9}, {29.4,19.6}, {30.0,18.5}, {29.0,18.9}, {29.0,21.4}, {28.8,24.8},
		{28.9,27.7}, {28.8,29.3}, {26.0,29.4},{23.7,29.4}, {23.9,26.2}, {23.8,24.3}, {22.9,24.1}, {23.2,26.5}, {23.1,29.2}, {20.4,29.7},
		{18.0,29.3}, {17.8,25.9}, {18.0,23.0},{18.1,20.6}, {18.1,19.0}, {17.3,18.2}, {16.9,18.9}, {17.4,20.2}, {16.4,21.7}, {16.4,20.3},
		{15.9,19.8}, {14.8,19.7}, {14.2,21.2},{13.8,21.3}};

// float KP;

float max(float a, float b){
	if(a > b)
		return a;
	return b;
}

/*****************************************
 * Complete this function so that it
 * continuously updates the robot's position
 *****************************************/
task trajectory_task()
{
	float t = 0.0;
	oldL = nMotorEncoder[LeftMotor];
	oldR = nMotorEncoder[RightMotor];
	oldT = nPgmTime;
	int loops = 0;
	while(1)
	{
		int curTime = nPgmTime;
		int leftEnc = nMotorEncoder[LeftMotor];
		int rightEnc = nMotorEncoder[RightMotor];
		float dt = (curTime - oldT) / 1000.0;
		t += dt;
		if(dt == 0.0){
			continue;
		}
		loops++;
		float vl = (leftEnc - oldL) / dt * PI / 180.0 * R;
		float vr = (rightEnc - oldR) / dt * PI / 180.0 * R;

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
		float k10 = v*cos(robot_TH + k02*.5*dt);
		float k11 = v*sin(robot_TH + k02*.5*dt);
		float k12 = w;
		float k20 = v*cos(robot_TH + k12*.5*dt);
		float k21 = v*sin(robot_TH + k12*.5*dt);
		float k22 = w;
		float k30 = v*cos(robot_TH + k22*dt);
		float k31 = v*sin(robot_TH + k22*dt);
		float k32 = w;

		robot_c.x += dt/6.0 * (k00 + 2*(k10 + k20) + k30);
		robot_c.y += dt/6.0 * (k01 + 2*(k11 + k21) + k31);
		robot_TH += dt/6.0 * (k02 + 2*(k12 + k22) + k32);


		/*
		float dl =

		float dd = (vl + vr) / 2.0;
		float dth = (vr-vl) / L;

		robot_c.x += dd * cos(robot_TH);
		robot_c.y += dd * sin(robot_TH);
		robot_TH += dth;*/

		marker_c.x = robot_c.x + F * cos(robot_TH);
		marker_c.y = robot_c.y + F * sin(robot_TH);

		trajectory(tn, t, marker_d);

		float xErr = marker_d.x - marker_c.x;
		float yErr = marker_d.y - marker_c.y;

		marker_vd = KP*(xErr*cos(robot_TH) + yErr*sin(robot_TH));
		marker_wd = KP*(xErr * -sin(robot_TH)/F + yErr * cos(robot_TH)/F);

		float vld = 90.0 / PI * (2.0*marker_vd - L*marker_wd) / R;
		float vrd = 90.0 / PI * (2.0*marker_vd + L*marker_wd) / R;

		motor[motorB] = 0.1 * vld;
		motor[motorC] = 0.1 * vrd;

		//Code that plots the robot's current position and also prints it out as text
		setPixel(50 + (int)(100.0 * marker_c.x), 32 + (int)(100.0 * marker_c.y));
		displayTextLine(0, "X: %f", marker_c.x);
		displayTextLine(1, "Y: %f", marker_c.y);
		displayTextLine(2, "xd: %f", marker_d.x);
		displayTextLine(3, "yd: %f", marker_d.y);
		displayTextLine(4, "t: %f", robot_TH * 180.0 / PI);
		displayTextLine(5, "x: %f", xErr);
		displayTextLine(6, "y: %f", yErr);

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
	p->x = 0.5 * cos(t/10.0) * sin(t/10.0) * 100;
	p->y = 0.2 * sin(t/10.0) * sin(t/5.0) * 100;
}

void traj2 (float t, point *p)
{
	t = clamp(t, 0.0, PI*10);
	p->x = 0.2 * sin(3*t/5) * 100;
	p->y = 0.2 * cos(2*(t/5 + PI/4)) * 100;
}

void traj3 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	p->x = 0.2 * cos(t/10) * cos(t/5) * 100;
	p->y = 0.2 * cos(3*t/10) * sin(t/10) * 100;
}

void traj4 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	p->x = 0.2 * (0.5 * cos(3*t/10) - 0.75 * cos(t/5)) * 100;
	p->y = 0.2 * (-0.75 * sin(t/5) - 0.5 * sin(3*t/10)) * 100;
}

void traj5 (float t, point *p)
{
	t = clamp(t, 0.0, PI*20);
	float c = cos(t/5);
	p->x = 0.1 * (-2*c*c - sin(t/10) + 1) * sin(t/5) * 100;
	p->y = 0.1 * c *(-2*c*c*c - sin(t/10) + 1) * 100;
}

void traj6 (float t, point *p)
{
	t = clamp(t, 0.0, PI*24);
	float c = cos(t/12);
	float s = sin(t/4);
	p->x = 0.1 * (2*c*c*c + 1)*s * 100;
	p->y = 0.1 * cos(t/4) * (1 - 2*s*s*s*s) * 100;
}

void traj7 (float t, point *p)
{
	t = clamp(t, 0.0, PI*40);
	p->x = 0.04 * (5*cos(9*t/20) - 4*cos(t/4)) * 100;
	p->y = 0.04 * (-4*sin(t/4) - 5*sin(9*t/20)) * 100;
}

void traj8 (float t, point *p)
{
	float tScale = 1.6;
	t = clamp(t, 0.0, (float)(numPoints-1) * tScale);

		int t0 = floor(t / tScale);
		int t1 = ceil(t/ tScale);
		float scale = (t/tScale) - t0;
		p->x = (1-scale)*points[t0][0] + scale*points[t1][0];
		p->y = (1-scale)*points[t0][1] + scale*points[t1][1];
		p->y = 40.0 - p->y;

		p->x = p->x * tScale;
		p->y = p->y * tScale;
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
		case 7:
		  traj8(t, p);
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
   int traj_count = 8;
   while (nNxtButtonPressed != kEnterButton)
   {
     displayTextLine(0, "Trajectory %d", (traj+1));
     if (nNxtButtonPressed == kLeftButton)
       traj--;
     else if (nNxtButtonPressed == kRightButton)
       traj++;

     if (traj < 0)
       traj = traj_count - 1;
     else if (traj > traj_count - 1)
       traj = 0;

     wait1Msec(150);
   }
   return traj;
 }

 float get_KP() {
   float k = 0.0;
   int i = 0;
	 nMotorEncoder[motorA] = 0;
	 nNxtButtonTask = 0;
   while(nNxtButtonPressed != kEnterButton) {
			k = nMotorEncoder[motorA] / 5.0;
			displayTextLine(0, "k = %f", k);
			wait10Msec(1);
	 }
	 nMotorEncoder[motorA] = 0;
	 wait10Msec(30);
	 return k;
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

	tn = get_trajectory();

	for (int i = 0; i < 7; i++) {
		nxtDisplayClearTextLine(i);
	}

	wait1Msec(500);

	//KP = get_KP();

	trajectory(tn, 0.0, marker_c);
	robot_c.x = marker_c.x - F;
	robot_c.y = marker_c.y;

	startTask(trajectory_task);

	while(nNxtButtonPressed != kExitButton) {}
}
