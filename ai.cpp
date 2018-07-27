// 到处转转，见人就打

#include "ai.h"

#include <cmath>

#include <iostream>

using namespace std;

static double cos_d(double theta) { return cos(theta / 180 * 3.14159); }
static double sin_d(double theta) { return sin(theta / 180 * 3.14159); }
static double tan_d(double theta) { return tan(theta / 180 * 3.14159); }
static double atan2_d(double y, double x) { return (abs(x)<1e-4 && abs(y)<1e-4) ? 0.0 : 180 / 3.14159*atan2(y, x); }
static double atan2_d(Point<double> target, Point<double> cur) { return (cur.getDistance(target) < 1e-4) ? 0.0 : 180 / 3.14159*atan2(target.y - cur.y, target.x - cur.x); }
static double asin_d(double s) { return 180 / 3.14159*asin(s); }
static double minus_angle_d(double t1, double t2) {
	double u = fmod(t1 - t2 + 360, 360);
	if (u > 180) u -= 360;
	return u;
}

// 双方选手的关键点
Point<double> target_coors[2][6] = {
	{
		Point<double>(200,225),
		Point<double>(320,280),
		Point<double>(250,150),
		Point<double>(200,75),
		Point<double>(20,20),
		Point<double>(20,280),
	},{
		Point<double>(200,225),
		Point<double>(80,20),
		Point<double>(150,150),
		Point<double>(200,75),
		Point<double>(380,20),
		Point<double>(380,280),
	} };
const int CNT_TARGET = 6;

Info car_angle;
double left_speed;
double right_speed;
static int target_id = 0;

//HP
Point<double>hp_coor[2][1] = {
	{
		Point<double>(20,20)
	},{
        Point<double>(380,280)
    }
    };

//MP
Point<double>mp_coor[2][1] = {
	{
		Point<double>(20,280)
	},{
		Point<double>(380,20)
		}
};
//tg
Point<double>tg_coor[2][1] = {
	{ Point<double>(320,280) },
{ Point<double>(80,20) }
};

//shd
Point<double>shd_coor = {
	{
		Point<double>(200,225),
}
};
// 给出前往指定地点的指令
PlayerControl rush_target(const Info info, Point<double> target_coor) {
	// 已经到了
	if (target_coor.getDistance(info.coor) < RADIUS_PROP) {
		return PlayerControl{ 75, -75, 0, NoAction };
	}
	
	TAngle A;
	TSpeed max_speed = 50;
	if (info.spd_status) max_speed = 75;
	else if (info.frz_status) max_speed = 25;
	TAngle target_angle = atan2_d(target_coor.y - info.coor.y, target_coor.x - info.coor.x);
	TAngle delta_angle = minus_angle_d(target_angle, info.car_angle);
	TAngle max_roate_angle = (max_speed / FREQ) / RADIUS_CAR / 3.14159 * 180;
	TSpeed ss = delta_angle * RADIUS_CAR * FREQ * 3.14159 / 180;
	if (info.cars.size() > 0 && info.coor.getDistance(info.cars[0].coor) > 40 && info.hp > 80)
	{
		A = target_angle = (atan2_d(info.cars[0].coor.y - info.coor.y, info.cars[0].coor.x - info.coor.x) - info.car_angle);
		target_coor = shd_coor;
	}
	else
	{
		A = 0;
	}
	if (abs(delta_angle) < max_roate_angle / 2) { // 直接跑
		if (delta_angle > 0)
			return PlayerControl{ max_speed - 2 * ss, max_speed, A, NoAction };
		else
			return PlayerControl{ max_speed, max_speed - 2 * ss, A, NoAction };
	}
	else if (abs(delta_angle) > max_roate_angle) { // 全速转
		if (delta_angle > 0)  // 左转
			return PlayerControl{ -25, 50, 0, NoAction };
		else  // 右转
			return PlayerControl{ 50 , -25, 0, NoAction };
	}
	else { // 指定角度转
		return PlayerControl{ -ss, ss, A, NoAction };
	}

}


// 如果遇到敌人就会攻击一会儿的AI
	PlayerControl attack_ai(const Info info) {
	PlayerControl pc;
	TAngle new_car_angle = info.car_angle + (-left_speed / FREQ + right_speed / FREQ) / 2 / RADIUS_CAR * 180 / 3.14159;
	// 移动：敌方，取道具，跑点
	Point<double> target_coor = info.coor;
	target_coor = shd_coor;
	if (info.cars.size() > 0 && info.shd_status)
	{
		target_coor = info.cars[0].coor;
		
	}
	else 
	{
		target_coor = shd_coor;
		if (info.cars.size() > 0)
			target_coor = info.cars[0].coor;
		
	}
	pc = rush_target(info, target_coor);
	// 攻击，防御，换弹
	if (info.can_spd && !info.spd_status)
		pc.action = SpeedUp;
	if (info.cars.size() > 0)
	{
		if (info.can_shd && !info.shd_status)
			pc.action = Shield;
		else
		{
			if (info.coor.getDistance(info.cars[0].coor) <= 90 && info.coor.getDistance(info.cars[0].coor) >= 0)
				pc.action = Attack3;
			if (info.coor.getDistance(info.cars[0].coor) <= 110 && info.coor.getDistance(info.cars[0].coor) >= 90)
				pc.action = Attack2;
			if (info.coor.getDistance(info.cars[0].coor) <= 130 && info.coor.getDistance(info.cars[0].coor) >= 110)
				pc.action = Attack1;
			if (info.mag == 1)
				pc.action = Attack1;
			if (info.mag == 2)
				pc.action = Attack2;
			if (info.mag == 3)
				pc.action = Attack3;
		}
	}
	if (info.mag <= 0 || info.cars.size() < 0)
		pc.action = ChangeMag;
	return pc;
}

// Main ---------- ---------- ---------- ---------- ----------

	PlayerControl player_ai(const Info info) {
	return attack_ai(info);

}