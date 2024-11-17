#ifndef MOCKSTORAGE_H
#define MOCKSTORAGE_H

using namespace std;

/* Instead of storing the files in the VM, we do it programatically.
 * Doing it here avoids each teammate needing to type in each file
 * individually into the VMBox terminal
 */

/*
 * To check collisions, duplicate an entry and change its ID.
 * If you have duplicate ID's, the program will crash.
 */

/*	Format of string:
 * 	EntryTime, ID, X, Y, Z, SpeedX, SpeedY, SpeedZ
 */

class MockStorage {
public:

	MockStorage(){};

	// Low traffic shouldnt have a predicted collision at all
	string lowTraffic =
		"0, 101, 5000, 2000, 15000, 250, -20, 30;\n"
		"4, 1350, 45000, 30000, 12500, -20, 255, 25;\n"
		"5, 1472, 80000, 14000, 11500, 0, 350, 35;\n"
		"5, 1586, 0, 0, 13000, 1000, 3000, 20;\n";

	// No predicted collision
	string mediumTraffic =
		"1, 101, 5000, 2000, 15000, 250, -20, 30;\n"
		"5, 1350, 45000, 30000, 12500, -20, 255, 25;\n"
		"7, 1472, 80000, 14000, 11500, 0, 350, 35;\n"
		"3, 1586, 8000, 75000, 13000, 260, -10, 20;\n"
		"9, 102, 0, 0, 20000, 0, 500, 0;\n"
		"4, 103, 0, 25000, 20000, 0, -500, 0;\n"
		"25, 104, 0, 28000, 5000, 0, -500, 0;\n";

	/*
	 *  Will have no collisions on start since all planes are flying in different directions
	 *  If you do "changepred 0" you'll see that the planes are currently in violation
	 */
	string highTraffic =
			"1, 101, 5000, 2000, 15000, 250, -20, 30;\n"
			"5, 1350, 45000, 30000, 12500, -20, 255, 25;\n"
			"7, 1472, 80000, 14000, 11500, 0, 350, 35;\n"
			"3, 1586, 8000, 75000, 13000, 260, -10, 20;\n"
			"9, 102, 0, 0, 20000, 0, 500, 0;\n"
			"4, 103, 0, 25000, 20000, 0, -500, 0;\n"
			"25, 104, 0, 28000, 25000, 0, -500, 0;\n"
			"4, 105, 5020, 2020, 9980, 250, -20, 29;\n"
			"5, 106, 5025, 2025, 9975, -20, 250, 30;\n"
			"6, 107, 5030, 2030, 9970, 125, 125, 28;\n";

	// Its a party in the corner of the map!
	string congestedTraffic =
		"0, 101, 5000, 2000, 10000, 250, -20, 30;\n"
		"1, 102, 5005, 2005, 9995, 252, -19, 29;\n"
		"2, 103, 5010, 2010, 9990, 253, -18, 28;\n"
		"3, 104, 5015, 2015, 9985, 251, -17, 30;\n"
		"4, 105, 5020, 2020, 9980, 250, -20, 29;\n"
		"5, 106, 5025, 2025, 9975, 252, -19, 30;\n"
		"6, 107, 5030, 2030, 9970, 253, -18, 28;\n"
		"7, 108, 5035, 2035, 9965, 251, -17, 27;\n"
		"8, 109, 5040, 2040, 9960, 252, -16, 29;\n"
		"9, 110, 5045, 2045, 9955, 250, -15, 30;\n"
		"10, 111, 5050, 2050, 9950, 253, -14, 28;\n"
		"11, 112, 5055, 2055, 9945, 252, -13, 29;\n"
		"12, 113, 5060, 2060, 9940, 251, -15, 30;\n"
		"13, 114, 5065, 2065, 9935, 250, -14, 29;\n"
		"14, 115, 5070, 2070, 9930, 252, -16, 27;\n"
		"15, 116, 5075, 2075, 9925, 253, -17, 28;\n"
		"16, 117, 5080, 2080, 9920, 251, -15, 30;\n"
		"17, 118, 5085, 2085, 9915, 250, -14, 29;\n"
		"18, 119, 5090, 2090, 9910, 252, -13, 28;\n"
		"19, 120, 5095, 2095, 9905, 253, -12, 27;\n";

};

#endif
