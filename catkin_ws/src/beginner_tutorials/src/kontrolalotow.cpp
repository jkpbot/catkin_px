#include "ros/ros.h"
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <sstream>
#include "std_msgs/String.h"

int cel=0;
mavros_msgs::State current_state;
class controller
{
    public:
    controller()
    {
        sub=n.subscribe("chatter",1000,&controller::callback,this);

        pub=n.advertise<std_msgs::String>("antohertopic",1000);
        
        state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10,&controller::state_cb,this);
        local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10);
        arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
        set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
        pozycja=n.subscribe("mavros/local_position/pose",1000,&controller::chatterCallback,this);

    }

    
    void state_cb(const mavros_msgs::State::ConstPtr& msg)
    {
    current_state = *msg;
    }
    void chatterCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
    {
    geometry_msgs::PoseStamped pose;
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"i heard as controller "<<msg->pose.position.x<<"  ,  "<<msg->pose.position.y<<"  ,  "<<msg->pose.position.z<<std::endl;
    pub_str.data=ss.str();
    std::cout<<pub_str.data.c_str()<<std::endl;
   
    switch (cel)
     {
      case 0: 
            pose.pose.position.x = 0;
            pose.pose.position.y = 0;
            pose.pose.position.z = 20;
            cel++;
         break;
      
      case 1: 
      if(msg->pose.position.z > 19)
        {
            pose.pose.position.x = 10;
            pose.pose.position.y = 15;
            pose.pose.position.z = 23;
            cel++;
        }break;
     
      
      case 2:
      if(msg->pose.position.x > 9 && msg->pose.position.x <11 && msg->pose.position.y >14 && msg->pose.position.y < 16 && msg->pose.position.z > 22 && msg->pose.position.z <24)
        {
        pose.pose.position.x = 30;
        pose.pose.position.y = 56;
        pose.pose.position.z = 33;
         cel++;
        }break;
        
      case 3:
      if(msg->pose.position.x > 29 && msg->pose.position.x <31 && msg->pose.position.y >55 && msg->pose.position.y < 57 && msg->pose.position.z > 32 && msg->pose.position.z <34)
        {
        pose.pose.position.x = 40;
        pose.pose.position.y = 26;
        pose.pose.position.z = 33;
         cel++;
        }break;
      
      case 4:
      if(msg->pose.position.x > 39 && msg->pose.position.x <41 && msg->pose.position.y >25 && msg->pose.position.y < 27 && msg->pose.position.z > 32 && msg->pose.position.z <34)
        {
        pose.pose.position.x = 0;
        pose.pose.position.y = 0;
        pose.pose.position.z = 20;
        cel=0;
        }break;
      } local_pos_pub.publish(pose);
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
ros::NodeHandle nh;
ros::Subscriber state_sub;
ros::Publisher local_pos_pub;
ros::ServiceClient arming_client;
ros::ServiceClient set_mode_client;
ros::Subscriber pozycja;

};
int main(int argc, char **argv)
{
    
    ros::init(argc,argv,"kontrolalotow");
    controller ctrl;
    ros::spin();
    return 0;
}