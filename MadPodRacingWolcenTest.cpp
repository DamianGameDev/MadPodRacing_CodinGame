#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define PI  (3.1415926535897932f)

/**
* Base Actor class which represents a point in space with a radius
*/
class BaseActor
{
protected:
	int ID; // Actor ID
	int Position_x;
	int Position_y;
	int Radius = 600;
public:
	int GetID() const { return ID; }
	int GetPositionX() const { return Position_x; }
	int GetPositionY() const { return Position_y; }
	int GetRadius() const { return Radius; }
	void SetID(const int NewID) { ID = NewID; }
	void SetPositionX(const int NewPosition) { Position_x = NewPosition; }
	void SetPositionY(const int NewPosition) { Position_y = NewPosition; }
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
	int LapsToFinish; // Number of laps to finish to end the race
	int LongestDistance = 0; // Longest distance from one checkpoint to another
	u_int8_t bCompletedLap : 1; // true if a lap was completed(once checkpoint got repeated), false otherwise

public:
	int GetLongestDistance() const { return LongestDistance; }
	void SetLapsToFinish(const int LapsNum) { LapsToFinish = LapsNum; }

	/** Called to Register a found Checkpoint */
	void RegisterCheckPoint(const int CheckPointPos_x, const int CheckPointPos_y, const bool bIsLastCheckPoint = false)
	{
		CheckPoint* NewCheckPoint = new CheckPoint();
		NewCheckPoint->SetPositionX(CheckPointPos_x);
		NewCheckPoint->SetPositionY(CheckPointPos_y);
		NewCheckPoint->SetID(Checkpoints.size());
		if (Checkpoints.size() > 0)
		{
			int DifF_X;
			int DifF_Y;
			int Distance;
			int CheckPointRefPos_x;
			int CheckPointRefPos_y;
			CheckPoint* PreviewsCheckPointRef = Checkpoints.back();
			if (PreviewsCheckPointRef)
			{
				CheckPointRefPos_x = PreviewsCheckPointRef->GetPositionX();
				CheckPointRefPos_y = PreviewsCheckPointRef->GetPositionY();

				DifF_X = CheckPointPos_x - CheckPointRefPos_x;
				DifF_Y = CheckPointPos_y - CheckPointRefPos_y;
				Distance = DifF_X * DifF_X + DifF_Y * DifF_Y;
				Distance = sqrt(Distance);
				NewCheckPoint->SetDistance(Distance);
				if (Distance > LongestDistance)
				{
					LongestDistance = Distance;
				}
			}
			if (bIsLastCheckPoint)
			{
				CheckPoint* StartCheckPointRef = Checkpoints[0];
				if (StartCheckPointRef)
				{
					CheckPointRefPos_x = StartCheckPointRef->GetPositionX();
					CheckPointRefPos_y = StartCheckPointRef->GetPositionY();

					DifF_X = CheckPointRefPos_x - CheckPointPos_x;
					DifF_Y = CheckPointRefPos_y - CheckPointPos_y;

					Distance = DifF_X * DifF_X + DifF_Y * DifF_Y;
					Distance = sqrt(Distance);
					StartCheckPointRef->SetDistance(Distance);
					if (Distance > LongestDistance)
					{
						LongestDistance = Distance;
					}
				}
			}
		}
		Checkpoints.push_back(NewCheckPoint);
	}

	/** Returns the stored checkpoint searching by their ID */
	CheckPoint* GetCheckPointByID(const int InID) const
	{
		for (CheckPoint* TestedCheckPoint : Checkpoints)
		{
			if (TestedCheckPoint->GetID() == InID)
			{
				return TestedCheckPoint;
			}
		}
		return nullptr;
	}
};

/**
* Class that represent a Pod in the game
*/
class Pod : public BaseActor
{
protected:
	int Thrust;
	int Speed_x;
	int Speed_y;
	int NextCheckPointID; // Next CheckPoint ID
	int WorldAngle; // Absolute Angle the pod is looking at
	float AngleToTarget; // Relative angle toward target
	float DistanceToTarget; // Distance to reach target
	CheckPointManager* CheckPointManagerRef; // CheckPoint Manager reference

public:
	int GetThrust() const { return Thrust; }
	int GetSpeedX() const { return Speed_x; }
	int GetSpeedY() const { return Speed_y; }
	void SetAngleToTarget(const int NewAngle) { WorldAngle = NewAngle; }
	void SetSpeedX(const int NewSpeed) { Speed_x = NewSpeed; }
	void SetSpeedY(const int NewSpeed) { Speed_y = NewSpeed; }
	void SetNextCheckPointId(const int NewID) { NextCheckPointID = NewID; }
	void SetCheckPointManagerRef(CheckPointManager* InCheckPointManager) { CheckPointManagerRef = InCheckPointManager; };

	/** Constructor */
	Pod()
	{
		Radius = 400;
	}

	/** Called to Update Pod and send output */
	void Update()
	{
		CheckPoint* NextCheckPointRef = CheckPointManagerRef->GetCheckPointByID(NextCheckPointID);
		const int next_checkpoint_x = NextCheckPointRef->GetPositionX();
		const int next_checkpoint_y = NextCheckPointRef->GetPositionY();

		UpdateDistanceToTarget(next_checkpoint_x, next_checkpoint_y);
		UpdateRelativeAngle(next_checkpoint_x, next_checkpoint_y);
		UpdateThrust();

		const int OffsetMultiplier = 3;
		const int TargetOffset_x = Speed_x * OffsetMultiplier;
		const int TargetOffset_y = Speed_y * OffsetMultiplier;

		if (ShouldBoost(NextCheckPointID))
		{
			cout << next_checkpoint_x - TargetOffset_x << " " << next_checkpoint_y - TargetOffset_y << " " << "BOOST" << endl;
		}
		else
		{
			cout << next_checkpoint_x - TargetOffset_x << " " << next_checkpoint_y - TargetOffset_y << " " << Thrust << endl;
		}
	}

	/** Called to update the distance to target of the pod */
	void UpdateDistanceToTarget(const int next_checkpoint_x, const int next_checkpoint_y)
	{
		CheckPoint* NextCheckPointRef = CheckPointManagerRef->GetCheckPointByID(NextCheckPointID);

		const int DifF_X = next_checkpoint_x - Position_x;
		const int DifF_Y = next_checkpoint_y - Position_y;

		const int SquaredDistance = DifF_X * DifF_X + DifF_Y * DifF_Y;

		DistanceToTarget = sqrt(SquaredDistance);
	}

	/** Called to update the relative anfle toward the target */
	void UpdateRelativeAngle(const int next_checkpoint_x, const int next_checkpoint_y)
	{
		const float PodPositionMagnitude = sqrt(Position_x * Position_x + Position_y * Position_y);
		const float PodNormalizedPosition_x = Position_x / PodPositionMagnitude;
		const float PodNormalizedPosition_y = Position_y / PodPositionMagnitude;
		cerr << "x: " << PodNormalizedPosition_x << endl;
		cerr << "y: " << PodNormalizedPosition_y << endl;
		const float CheckPointPositionMagnitude = sqrt(next_checkpoint_x * next_checkpoint_x + next_checkpoint_y * next_checkpoint_y);
		const float CheckPointNormalizedPosition_x = next_checkpoint_x / CheckPointPositionMagnitude;
		const float CheckPointNormalizedPosition_y = next_checkpoint_y / CheckPointPositionMagnitude;
		const float DotProduct = PodNormalizedPosition_x * CheckPointNormalizedPosition_x + PodNormalizedPosition_y * CheckPointNormalizedPosition_y;

		AngleToTarget = atan2((next_checkpoint_y - Position_y), (next_checkpoint_x - Position_x)) * 180 / PI;
		AngleToTarget = AngleToTarget - WorldAngle;
		if (WorldAngle >= 183)
		{
			AngleToTarget += 360;
		}
		cerr << "Angle:" << AngleToTarget << endl;
		cerr << "World:" << WorldAngle << endl;
	}

	/** Called to Update the Pod's thrust with some criteria */
	void UpdateThrust()
	{
		float ThrustMultiplierProximity = DistanceToTarget / (2 * 600); // start decreasing thrust when distance is less than Twice the checkpoint size
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
			Thrust = 100 * ThrustMultiplierAngle * ThrustMultiplierProximity;
		}
	}

	/** Returns true if the distance to the next checkpoint is the longest on the lap */
	bool ShouldBoost(const int NextCheckPointID) const
	{
		if (CheckPointManagerRef)
		{
			int LongestDistance = CheckPointManagerRef->GetLongestDistance();
			CheckPoint* NextCheckPoint = CheckPointManagerRef->GetCheckPointByID(NextCheckPointID);
			cerr << "LongestDistance:" << LongestDistance << endl; // TODO: Erase debugging code on final
			cerr << "CheckPointDistance:" << NextCheckPoint->GetDistance() << endl; // TODO: Erase debugging code on final
			if (NextCheckPoint && NextCheckPoint->GetDistance() >= LongestDistance && AngleToTarget <= 15.0f && AngleToTarget >= -15.0f)
			{
				cerr << "BOOSTING!" << endl; // TODO: Erase debugging code on final
				return true;
			}
		}
		return false;
	}
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
	CheckPointManager* MyCheckPointManager = new CheckPointManager();

	int laps;
	cin >> laps; cin.ignore();
	MyCheckPointManager->SetLapsToFinish(laps);

	int checkpoint_count;
	cin >> checkpoint_count; cin.ignore();
	for (int i = 0; i < checkpoint_count; i++) {
		int checkpoint_x;
		int checkpoint_y;
		cin >> checkpoint_x >> checkpoint_y; cin.ignore();

		MyCheckPointManager->RegisterCheckPoint(checkpoint_x, checkpoint_y, i == checkpoint_count - 1);
	}

	vector<Pod*> MyPods;
	Pod* MyPodA = new Pod();
	Pod* MyPodB = new Pod();
	MyPodA->SetID(1);
	MyPodB->SetID(2);
	MyPodA->SetCheckPointManagerRef(MyCheckPointManager);
	MyPodB->SetCheckPointManagerRef(MyCheckPointManager);
	MyPods.push_back(MyPodA);
	MyPods.push_back(MyPodB);

	int FrameCounter = 0;
	// game loop
	while (1) {
		for (int i = 0; i < 2; i++) {
			int x; // x position of your pod
			int y; // y position of your pod
			int vx; // x speed of your pod
			int vy; // y speed of your pod
			int angle; // angle of your pod
			int next_check_point_id; // next check point id of your pod
			cin >> x >> y >> vx >> vy >> angle >> next_check_point_id; cin.ignore();

			MyPods[i]->SetPositionX(x);
			MyPods[i]->SetPositionY(y);
			MyPods[i]->SetSpeedX(vx);
			MyPods[i]->SetSpeedY(vy);
			MyPods[i]->SetAngleToTarget(angle);
			MyPods[i]->SetNextCheckPointId(next_check_point_id);
		}
		for (int i = 0; i < 2; i++) {
			int x_2; // x position of the opponent's pod
			int y_2; // y position of the opponent's pod
			int vx_2; // x speed of the opponent's pod
			int vy_2; // y speed of the opponent's pod
			int angle_2; // angle of the opponent's pod
			int next_check_point_id_2; // next check point id of the opponent's pod
			cin >> x_2 >> y_2 >> vx_2 >> vy_2 >> angle_2 >> next_check_point_id_2; cin.ignore();
		}

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// You have to output the target position
		// followed by the power (0 <= thrust <= 100)
		// i.e.: "x y thrust"

		for (Pod* CurrentPod : MyPods)
		{
			cerr << endl << "Debug Pod_" << CurrentPod->GetID() << endl;
			CurrentPod->Update();
		}
	}
	// Free space on gameloop end
	delete MyCheckPointManager;
	delete MyPodA;
	delete MyPodB;
}