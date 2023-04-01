#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

class controller
{
    public:
    controller()
    {
        sub=n.subscribe("chatter",1000,&controller::callback,this);
        pub=n.advertise<std_msgs::String>("antohertopic",1000);
    }
    void callback(const std_msgs::String::ConstPtr& msg)
    {
        std_msgs::String pub_str;
        std::stringstream ss;
        ss<<"i heard as controller "<<msg->data.c_str();
        pub_str.data=ss.str();
        std::cout<<pub_str.data.c_str()<<std::endl;
        pub.publish(pub_str);
        ros::spinOnce();
    }
private:
ros::NodeHandle n;
ros::Subscriber sub;
ros::Publisher pub;
};
int main(int argc, char **argv)
{
    
    ros::init(argc,argv,"contoroller");
    controller ctrl;
    ros::spin();
    return 0;
}