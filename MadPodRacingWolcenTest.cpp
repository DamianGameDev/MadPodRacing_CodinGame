#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
* Base Actor class which represents a point in space with a radius
*/
class BaseActor
{
protected:
	int Position_x;
	int Position_y;
	int Radius = 600;
public:
	int GetPositionX() const { return Position_x; }
	int GetPositionY() const { return Position_y; }
	int GetRadius() const { return Radius; }
	virtual void SetPositionX(const int NewPosition) { Position_x = NewPosition; }
	virtual void SetPositionY(const int NewPosition) { Position_y = NewPosition; }
};

/**
* Class that represents a Checkpoint
*/
class CheckPoint : public BaseActor
{
protected:
	int Distance; // Distance from previous checkpoint

public:
	int GetDistance() const { return Distance; }
	void SetDistance(const int NewDistance) { Distance = NewDistance; }
};

/**
* Class manager which stores known checkpoints and when a lap is completed
*/
class CheckPointManager
{
	vector<CheckPoint*> Checkpoints; // Vector storing the known checkpoints
	int SavedLocation[2] = {}; // Last location where a checkpoint was found
	int LongestDistance; // Longest distance from one checkpoint to another
	u_int8_t bCompletedLap : 1; // true if a lap was completed(once checkpoint got repeated), false otherwise
	u_int8_t bAlreadyInitialized : 1; // true if the class was already initialized, false otherwise

public:
	/** Called to Initialize FirstLocation */
	void InitializeManager(const int CheckPointPos_x, const int CheckPointPos_y)
	{
		if (!bAlreadyInitialized)
		{
			bAlreadyInitialized = true;
			SavedLocation[0] = CheckPointPos_x;
			SavedLocation[1] = CheckPointPos_y;
		}
	}

	/** Called to Register a found Checkpoint */
	void RegisterCheckPoint(CheckPoint* NewCheckPoint)
	{
		if (!bCompletedLap)
		{
			for (CheckPoint* TestCheckPoint : Checkpoints)
			{
				if (TestCheckPoint->GetPositionX() == NewCheckPoint->GetPositionX() && TestCheckPoint->GetPositionY() == NewCheckPoint->GetPositionY())
				{
					cerr << "CompletedLap:" << endl; // TODO: Erase debugging code on final
					bCompletedLap = true;
					break;
				}
			}
			if (!bCompletedLap)
			{
				Checkpoints.push_back(NewCheckPoint);
				const int CheckPointDistance = NewCheckPoint->GetDistance();
				if (CheckPointDistance > LongestDistance)
				{
					LongestDistance = CheckPointDistance; //TODO set this by comparing to next checkpoint
				}
			}
		}
	}

	/** Returns true if the distance to the next checkpoint is the longest on the lap */
	bool ShouldBoost(CheckPoint* NextCheckPoint, const int Angle) const
	{
		cerr << "LongestDistance:" << LongestDistance << endl; // TODO: Erase debugging code on final
		cerr << "CheckPointDistance:" << NextCheckPoint->GetDistance() << endl; // TODO: Erase debugging code on final
		if (bCompletedLap)
		{
			if (NextCheckPoint->GetDistance() >= LongestDistance && Angle <= 15 && Angle >= -15)
			{
				return true;
			}
		}
		return false;
	}

	/** Returns the distance to the next checkpoint */
	float GetDistanceToNextCheckPoint(const int PosA[2], const int PosB[2])
	{
		const int Difference_x = PosA[0] - PosB[0];
		const int Difference_y = PosA[1] - PosB[1];
		int Sum = (Difference_x * Difference_x) + (Difference_y * Difference_y);
		float Result = sqrt(Sum);
		return Result;
	}

	/** Called to update and initialize a checkpoint if not known */
	void UpdateFoundCheckPoint(const int CheckPointPos_x, const int CheckPointPos_y, const int CheckPointDistance, CheckPointManager* InManager, CheckPoint*& OutCheckPoint)
	{
		if (SavedLocation[0] != CheckPointPos_x || SavedLocation[1] != CheckPointPos_y)
		{
			const int CheckpointPos[2] = { CheckPointPos_x, CheckPointPos_y };
			const int Distance = GetDistanceToNextCheckPoint(SavedLocation, CheckpointPos);
			CheckPoint* NewCheckPoint = new CheckPoint();
			NewCheckPoint->SetPositionX(CheckPointPos_x);
			NewCheckPoint->SetPositionY(CheckPointPos_y);
			NewCheckPoint->SetDistance(Distance);
			InManager->RegisterCheckPoint(NewCheckPoint);
			OutCheckPoint = NewCheckPoint;
		}

		SavedLocation[0] = CheckPointPos_x;
		SavedLocation[1] = CheckPointPos_y;
	}
};

/**
* Class that represent a Pod in the game
*/
class Pod : public BaseActor
{
protected:
	float Thrust;
	float AngleToTarget;
	float DistanceToTarget;
	int Speed_x;
	int Speed_y;

public:
	int GetThrust() const { return Thrust; }
	int GetSpeedX() const { return Speed_x; }
	int GetSpeedY() const { return Speed_y; }
	void SetAngleToTarget(const int NewAngle) { AngleToTarget = NewAngle; }
	void SetDistanceToTarget(const int NewDistance) { DistanceToTarget = NewDistance; }

	/** Constructor */
	Pod()
	{
		Radius = 400;
	}

	/** Called to set the Pod's thrust with some criteria */
	void SetThrust()
	{
		float ThrustMultiplierProximity = DistanceToTarget / (2 * 600); // start decreasing thrust when distance is less than Twice the checkpoint size
		cerr << "ClampedThrustMultiplierProximity:" << ThrustMultiplierProximity << endl;  // TODO: Erase debugging code on final
		if (ThrustMultiplierProximity > 1.0f)
		{
			ThrustMultiplierProximity = 1.0f;
		}
		if (AngleToTarget <= 5 && AngleToTarget >= -5)
		{
			Thrust = 100 * ThrustMultiplierProximity;
		}
		else
		{
			const float AngleCeiling = AngleToTarget / 90.0f; // start decreasing thrust when Angle is less than 90° (or higher than -90°)
			float ThrustMultiplierAngle = AngleCeiling < 0 ? 1.0f + AngleCeiling : 1.0f - AngleCeiling;
			if (ThrustMultiplierAngle < 0.1f)
			{
				ThrustMultiplierAngle = 0.1f; // Angle limit should not make thrust get under 10%
			}
			else if (AngleCeiling > 1.0f)
			{
				ThrustMultiplierAngle = 1.0f;
			}
			cerr << "ClampedThrustMultiplierAngle:" << ThrustMultiplierAngle << endl; // TODO: Erase debugging code on final
			Thrust = 100 * ThrustMultiplierAngle * ThrustMultiplierProximity;
		}
	}

	/** Called to set position and velocity in X */
	virtual void SetPositionX(const int NewPosition) override
	{
		Speed_x = NewPosition - Position_x;
		Position_x = NewPosition;
	}

	/** Called to set position and velocity in Y */
	virtual void SetPositionY(const int NewPosition) override
	{
		Speed_y = NewPosition - Position_y;
		Position_y = NewPosition;
	}
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
	CheckPointManager* MyCheckPointManager = new CheckPointManager();
	CheckPoint* CurrentCheckpoint = new CheckPoint();
	Pod* MyPod = new Pod();
	int FrameCounter = 0;
	// game loop
	while (1) {
		int x;
		int y;
		int next_checkpoint_x; // x position of the next check point
		int next_checkpoint_y; // y position of the next check point
		int next_checkpoint_dist; // distance to the next checkpoint
		int next_checkpoint_angle; // angle between your pod orientation and the direction of the next checkpoint
		cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >> next_checkpoint_dist >> next_checkpoint_angle; cin.ignore();
		int opponent_x;
		int opponent_y;
		cin >> opponent_x >> opponent_y; cin.ignore();

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// You have to output the target position
		// followed by the power (0 <= thrust <= 100)
		// i.e.: "x y thrust"
		MyCheckPointManager->InitializeManager(next_checkpoint_x, next_checkpoint_y);
		MyPod->SetAngleToTarget(next_checkpoint_angle);
		MyPod->SetDistanceToTarget(next_checkpoint_dist);
		MyPod->SetPositionX(x);
		MyPod->SetPositionY(y);
		MyPod->SetThrust();
		int CheckPointRadius = CurrentCheckpoint->GetRadius();
		int thrust = MyPod->GetThrust();
		int OffsetMultiplier = 3;
		int TargetOffset_x = FrameCounter > 1 ? MyPod->GetSpeedX() * OffsetMultiplier : 0;
		int TargetOffset_y = FrameCounter > 1 ? MyPod->GetSpeedY() * OffsetMultiplier : 0;
		while (FrameCounter <= 1) // This was hardcoded since position on first two frames gave a weird speed (high value)
		{
			FrameCounter++;
		}

		cerr << "Speed_x:" << MyPod->GetSpeedX() << endl << "speed_y:" << MyPod->GetSpeedY() << endl; // TODO: Erase debugging code on final
		cerr << "TargetOffset_x:" << TargetOffset_x << endl << "TargetOffset_y:" << TargetOffset_y << endl; // TODO: Erase debugging code on final
		MyCheckPointManager->UpdateFoundCheckPoint(next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, MyCheckPointManager, CurrentCheckpoint);

		if (MyCheckPointManager->ShouldBoost(CurrentCheckpoint, next_checkpoint_angle) || (next_checkpoint_dist >= 10000 && next_checkpoint_angle <= 5 && next_checkpoint_angle >= -5))
		{
			cout << next_checkpoint_x - TargetOffset_x << " " << next_checkpoint_y - TargetOffset_y << " " << "BOOST" << endl;
			cerr << "BOOSTING!" << endl; // TODO: Erase debugging code on final
		}
		else
		{
			cout << next_checkpoint_x - TargetOffset_x << " " << next_checkpoint_y - TargetOffset_y << " " << thrust << endl;
		}
		cerr << "Distance:" << next_checkpoint_dist << endl << "Angle:" << next_checkpoint_angle << endl << "Thrust:" << thrust << endl; // TODO: Erase debugging code on final      
	}
	// Free space on gameloop end
	delete MyCheckPointManager;
	delete CurrentCheckpoint;
	delete MyPod;
}