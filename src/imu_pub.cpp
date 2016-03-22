#include "vector.h"
#include "MinIMU9.h"
#include "version.h"
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <system_error>
#include <boost/program_options.hpp>
#include <unistd.h>
#include "minimu9-ahrs.h"

int main(int argc, char *argv[])
{
	MinIMU9 imu("/dev/i2c-1");
	ahrs(imu, &fuse_default, output_quaternion);
	return 0;
}
