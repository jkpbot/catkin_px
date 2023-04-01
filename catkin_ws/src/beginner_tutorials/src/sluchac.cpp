#include "ros/ros.h"
#include "std_msgs/String.h"


void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
	std::cout<<"slysze jako mavros : "<<msg->data.c_str()<<std::endl;
}

int main (int argc, char **argv)
{
	ros::init(argc,argv,"lisner");
	ros::NodeHandle n;
	ros::Subscriber sub=n.subscribe("mavros/altitude",1000,chatterCallback);
	ros::spin();
	return 0;
}
