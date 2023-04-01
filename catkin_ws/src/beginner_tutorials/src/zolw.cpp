#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <math.h>
#include "turtlesim/Pose.h"
#include "geometry_msgs/Twist.h"

class controller
{
    public:
    controller()
    {
        sub=n.subscribe("turtle1/pose",1000,&controller::callback,this);
        subrot=n.subscribe("turtle1/cmd_vel",1000,&controller::callback2,this);
        pub=n.advertise<geometry_msgs::Twist>("turtle1/cmd_vel",1000);
    }
    void callback2(const geometry_msgs::Twist::ConstPtr& msg)
    {
        std_msgs::String pub_str;
        std::stringstream ss;
        ss<<"obrot: "<<msg->angular.z<<std::endl;;
        pub_str.data=ss.str();
        std::cout<<pub_str.data.c_str()<<std::endl;
    }
    void callback(const turtlesim::Pose::ConstPtr& msg)
    {
        std_msgs::String pub_str;
        std::stringstream ss;
        ss<<"i heard as controller "<<msg->x<<"  ,  "<<msg->y;
        pub_str.data=ss.str();
        std::cout<<pub_str.data.c_str()<<std::endl;

        if(msg->x >=10 || msg->x <=1 || msg-> y>=10 ||msg->y<=1)
        {
            std::cout<<"1"<<std::endl;
            geometry_msgs::Twist rotate;
            float obr;
            obr=sqrt((msg->x-6)*(msg->x-6)+(msg->y-6)*(msg->y-6));
            rotate.angular.z=obr;
            pub.publish(rotate);
        }
         else 
        {
            std::cout<<"1"<<std::endl;
            geometry_msgs::Twist rotate;
             float obr;
            obr=sqrt((msg->x-6)*(msg->x-6)+(msg->y-6)*(msg->y-6));
            rotate.angular.z=-obr;
            pub.publish(rotate);
        }


    }
private:
ros::NodeHandle n;
ros::Subscriber sub;
ros::Subscriber subrot;
ros::Publisher pub;
};
int main(int argc, char **argv)
{
    
    ros::init(argc,argv,"contoroller");
    controller ctrl;
    ros::spin();
    return 0;
}