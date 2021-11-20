#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

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
	void SetPositionX(const int NewPosition) { Position_x = NewPosition; }
	void SetPositionY(const int NewPosition) { Position_y = NewPosition; }
};

class CheckPoint : public BaseActor
{
protected:
	int Distance;

public:
	int GetDistance() const { return Distance; }
	void SetDistance(const int NewDistance) { Distance = NewDistance; }
};

class CheckPointManager
{
	vector<CheckPoint*> Checkpoints;
	int SavedLocation[2] = {};
	int LongestDistance;
	u_int8_t bCompletedLap : 1;
	u_int8_t bAlreadyInitialized : 1;

public:
	void InitializeManager(const int CheckPointPos_x, const int CheckPointPos_y)
	{
		if (!bAlreadyInitialized)
		{
			bAlreadyInitialized = true;
			SavedLocation[0] = CheckPointPos_x;
			SavedLocation[1] = CheckPointPos_y;
		}
	}

	void RegisterCheckPoint(CheckPoint* NewCheckPoint)
	{
		if (!bCompletedLap)
		{
			for (CheckPoint* TestCheckPoint : Checkpoints)
			{
				if (TestCheckPoint->GetPositionX() == NewCheckPoint->GetPositionX() && TestCheckPoint->GetPositionY() == NewCheckPoint->GetPositionY())
				{
					cerr << "CompletedLap:" << endl;
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

	bool ShouldBoost(CheckPoint* NextCheckPoint, const int Angle) const
	{
		cerr << "LongestDistance:" << LongestDistance << endl;
		cerr << "CheckPointDistance:" << NextCheckPoint->GetDistance() << endl;
		if (bCompletedLap)
		{
			if (NextCheckPoint->GetDistance() >= LongestDistance && Angle <= 15 && Angle >= -15)
			{
				return true;
			}
		}

		return false;
	}

	float GetDistanceToNextCheckPoint(const int PosA[2], const int PosB[2])
	{
		const int Difference_x = PosA[0] - PosB[0];
		const int Difference_y = PosA[1] - PosB[1];
		int Sum = (Difference_x * Difference_x) + (Difference_y * Difference_y);
		float Result = sqrt(Sum);
		return Result;
	}

	void UpdateCheckpointCounter(const int CheckPointPos_x, const int CheckPointPos_y, const int CheckPointDistance, CheckPointManager* InManager, CheckPoint*& OutCheckPoint)
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

class Pod : public BaseActor
{
protected:
	float Thrust;
	float AngleToTarget;
	float DistanceToTarget;

public:
	int GetThrust() const { return Thrust; }
	void SetAngleToTarget(const int NewAngle) { AngleToTarget = NewAngle; }
	void SetDistanceToTarget(const int NewDistance) { DistanceToTarget = NewDistance; }

	Pod()
	{
		Radius = 400;
	}

	void SetThrust()
	{
		float ThrustMultiplierProximity = DistanceToTarget / (2 * 600); // Double the Checkpoint radius
		cerr << "ClampedThrustMultiplierProximity:" << ThrustMultiplierProximity << endl;
		if (ThrustMultiplierProximity > 1.0f)
		{
			ThrustMultiplierProximity = 1.0f;
		}
		if (AngleToTarget <= 1 && AngleToTarget >= -1)
		{
			Thrust = 100 * ThrustMultiplierProximity;
		}
		else
		{
			const float NormalizedAngle = AngleToTarget / 90.0f;
			float ThrustMultiplierAngle = NormalizedAngle < 0 ? 1.0f + NormalizedAngle : 1.0f - NormalizedAngle;
			if (ThrustMultiplierAngle < 0.0f)
			{
				ThrustMultiplierAngle = 0.0f;
			}
			else if (NormalizedAngle > 1.0f)
			{
				ThrustMultiplierAngle = 1.0f;
			}
			cerr << "ClampedThrustMultiplierAngle:" << ThrustMultiplierAngle << endl;
			Thrust = 100 * ThrustMultiplierAngle * ThrustMultiplierProximity;
		}
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
		MyPod->SetThrust();
		int CheckPointRadius = CurrentCheckpoint->GetRadius();
		int thrust = MyPod->GetThrust();

		MyCheckPointManager->UpdateCheckpointCounter(next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, MyCheckPointManager, CurrentCheckpoint);

		if (MyCheckPointManager->ShouldBoost(CurrentCheckpoint, next_checkpoint_angle))
		{
			cout << next_checkpoint_x << " " << next_checkpoint_y << " " << "BOOST" << endl;
			cerr << "BOOSTING!" << endl;
		}
		else
		{
			cout << next_checkpoint_x << " " << next_checkpoint_y << " " << thrust << endl;
		}
		cerr << "Distance:" << next_checkpoint_dist << endl << "Angle:" << next_checkpoint_angle << endl << "Thrust:" << thrust << endl;
	}
	delete MyCheckPointManager;
	delete CurrentCheckpoint;
	delete MyPod;
}