#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int LongestDistance = 1000; // SavedLongest distance, used to use Boost on an optimal way
int CheckPointCounter = 0; // Number of checkpoints reached
int SavedLocation[2] = {}; // Saved Checkpoints position, used to check if checkpoint changed

/** Returns true if the difference between two values are less than the given tolerance */
bool IsNearlyEqual(int A, int B, int Tolerance)
{
	return (abs(A - B) < Tolerance);
}

int main()
{

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

		if (SavedLocation[0] != next_checkpoint_x || SavedLocation[1] != next_checkpoint_y)
		{
			CheckPointCounter++;
		}

		SavedLocation[0] = next_checkpoint_x;
		SavedLocation[1] = next_checkpoint_y;

		int thrust = 100;
		if (next_checkpoint_dist > 700 && next_checkpoint_dist <= 900)
		{
			thrust = 30;
		}
		else if ((next_checkpoint_angle > 90 || next_checkpoint_angle < -90) && next_checkpoint_dist > 1500)
		{
			thrust = 10;
		}
		else
		{
			thrust = 100;
		}

		if (next_checkpoint_dist > LongestDistance)
		{
			LongestDistance = next_checkpoint_dist;
		}

		if (CheckPointCounter >= 4 && IsNearlyEqual(LongestDistance, next_checkpoint_dist, 1000) && next_checkpoint_angle <= 10 && next_checkpoint_angle >= -10)
		{
			cout << next_checkpoint_x << " " << next_checkpoint_y << " " << "BOOST" << endl;
			cerr << "BOOSTING!" << endl;
		}
		else
		{
			cout << next_checkpoint_x << " " << next_checkpoint_y << " " << thrust << endl;
		}
		cerr << "Distance:" << next_checkpoint_dist << endl << "Angle:" << next_checkpoint_angle << endl << "Lap:" << CheckPointCounter << endl << "LongestDistance:" << LongestDistance << endl;
	}
}