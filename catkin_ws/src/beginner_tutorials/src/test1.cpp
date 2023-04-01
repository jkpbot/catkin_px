#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include "std_msgs/String.h"

void chatterCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"i heard as controller "<<msg->pose.position.x<<"  ,  "<<msg->pose.position.y<<"  ,  "<<msg->pose.position.z<<std::endl;
        pub_str.data=ss.str();
        std::cout<<pub_str.data.c_str()<<std::endl;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "test1");
    ros::NodeHandle n;
	ros::Subscriber pozycja=n.subscribe("/mavros/local_position/pose",1000,chatterCallback);
    ros::spin();
    
  

    return 0;
}