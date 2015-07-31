// Mychar Script

double x = 160.0, y = 120.0;
double vx, vy;
double angle = 0.0;

// adjustable variables
const double jumpForce = 5.5;
const double gravity = 0.4;
const double accel = 0.4;
const double maxSpeed = 3.0;
const double friction = 0.95;

// key config
bool prevJumpTriggered = false;
bool isJumpTriggered
{
	get
	{
		bool rval = false;
		if(!prevJumpTriggered && getkey(32)) rval = true;
		prevJumpTriggered = getkey(32);
		return rval;
	}
}
bool hasLeftMoved
{
	get { return getLeftKey(); }
}
bool hasRightMoved
{
	get { return getRightKey(); }
}

void main()
{
	vx = 0.0;
	vy = 0.0;
	x = 160.0;
  	y = 120.0;
	angle = 0.0;
	print("Initialized Player");

	while(!_isGameOvered)
	{
		Move();
		if(getAt(x, y + 8) == 2)
		{
			vy = 0.0;
			y = floor(y / 16) * 16 + 8;
			vx = vx * friction;
			angle = 0.0;
			if(isJumpTriggered)
			{
				playJumpSound();
				vy = -jumpForce;
			}
		}
		else
		{
			angle = -30.0f;
		}
		vy += gravity;
		x += vx;
		y += vy;
		if(x < _viewX + 12) x = _viewX + 12;
		update();	// yield module execution
	}

	while(true)
	{
		angle -= 16.0f;
		update();
	}
}
void Move()
{
	if(hasLeftMoved)
	{
		vx -= accel;
	}
	if(hasRightMoved)
	{
		vx += accel;
	}
	if(abs(vx) > maxSpeed) vx = sign(vx) * maxSpeed;
}
