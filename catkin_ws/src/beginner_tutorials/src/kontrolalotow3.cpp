#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/NavSatFix.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/SetMode.h>
#include <geographic_msgs/GeoPoseStamped.h>
#include "std_msgs/String.h"
#include <sstream>
#include <math.h>

#include <GeographicLib/Geocentric.hpp>

#include <tf2/LinearMath/Quaternion.h>


int cel=0;
geometry_msgs::PoseStamped zadana;



float pozycja_poprawkowa_x;
float pozycja_poprawkowa_y;

float obrot;

double poprawka_x;
double poprawka_y;
double poprawka_z;


const float PI=22/7;

sensor_msgs::NavSatFix scigany_pozycaj;
sensor_msgs::NavSatFix poszukiwacz_pozycaj;


mavros_msgs::State status;

mavros_msgs::State current_state;



float azi1;

float to_degrees(float radians) 
{
    return radians * (180/PI);
}

float to_radians(float degrees) 
{
    return degrees * (PI/180);
}

float radians(float radians) 
{
    return radians * (180/PI);
}

float degrees(float degrees) 
{
    return degrees * (PI/180);
}

float distance(float lat,float lon,float lat2,float lon2)
{

    float R = 6371000; // metres
    float F1 = lat * PI/180; // φ, λ in radians
    float F2 = lat2 * PI/180;
    float delF = (lat2-lat) * PI/180;
    float delL = (lon2-lon) * PI/180;

    float a = (sin(delF/2) * sin(delF/2) +cos(F1) * cos(F2) *sin(delL/2) * sin(delL/2));
    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    float d = R * c; // in metres
  
    
    return d;
}

float bearing(float lat,float lon,float lat2,float lon2)
{
//lat = your current gps latitude.
//lon = your current gps longitude.
//lat2 = your destiny gps latitude.
//lon2 = your destiny gps longitude.

    float teta1 = radians(lat);
    float teta2 = radians(lat2);

    float delta1 = radians(lat2-lat);
    float delta2 = radians(lon2-lon);

    //==================Heading Formula Calculation================//

    float y = sin(delta2) * cos(teta2);
    float x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);


    float brng = atan2(y,x);
    brng = radians(brng);// radians to degrees
    
    return brng;
  }

float delta_x(float distance, float bearing)
{
    float delta_x=distance*sin(to_radians(bearing));
    return delta_x;
}

float delta_y(float distance, float bearing)
{
    float delta_y=distance*cos(to_radians(bearing));
    return delta_y;
}

void state_cb(const mavros_msgs::State::ConstPtr& msg)
{
    std_msgs::String pub_str;
    std::stringstream ss;
    //ss<<"slysze z statea "<<msg->armed<<std::endl;
   // pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
    current_state = *msg;
}


void chatterCallback_scigany(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"pozycja sciganego lat lon "<<msg->latitude<<"  ,  "<<msg->longitude;
    scigany_pozycaj= *msg;
    // pose.pose.position.latitude  = msg->latitude;
    // pose.pose.position.longitude = msg->longitude;
    // pose.pose.position.altitude = msg->altitude + 1;

    //pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
}

void chatterCallback_poszukiwacz_local(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    std_msgs::String pub_str;
    std::stringstream ss;
    pozycja_poprawkowa_x=delta_x(distance(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude),bearing(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude));
    
    pozycja_poprawkowa_y=delta_y(distance(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude),bearing(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude));
    
    ss<<"deltay is : "<<pozycja_poprawkowa_y<<std::endl;
    pub_str.data=ss.str();
  
    ss<<"deltax is : "<<pozycja_poprawkowa_x<<std::endl;
    pub_str.data=ss.str();
    

    ss<<"distance is : "<<distance(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude)<<std::endl;
    pub_str.data=ss.str();
   
    obrot=bearing(poszukiwacz_pozycaj.latitude,poszukiwacz_pozycaj.longitude,scigany_pozycaj.latitude,scigany_pozycaj.longitude);
    ss<<"angle is : "<<obrot<<std::endl;
    pub_str.data=ss.str();
    

    zadana.pose.position.x =msg->pose.position.x-pozycja_poprawkowa_x;
    zadana.pose.position.y =pozycja_poprawkowa_y+msg->pose.position.y;
    zadana.pose.position.z = 15; 
    // zadana.pose.position.x =0;
    // zadana.pose.position.y =0;
    // zadana.pose.position.z = 15; 
    tf2::Quaternion quaternion_;
    quaternion_.setRPY(0,0,to_radians(90+obrot));
    quaternion_ = quaternion_.normalize();



    ss<<"obr z  : "<<quaternion_.x()<<std::endl;
    pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
    ss<<"obr y  : "<<quaternion_.y()<<std::endl;
    pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
    ss<<"obr z  : "<<quaternion_.z()<<std::endl;
    pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
    ss<<"obr w  : "<<quaternion_.w()<<std::endl;
    pub_str.data=ss.str();
    std::cout<<pub_str.data.c_str()<<std::endl;


    zadana.pose.orientation.x = quaternion_.x();
    zadana.pose.orientation.y = quaternion_.y();
    zadana.pose.orientation.z = quaternion_.z();
    zadana.pose.orientation.w = quaternion_.w();



}

void chatterCallback_poszukiwacz(const sensor_msgs::NavSatFix::ConstPtr& msg)
{
    std_msgs::String pub_str;
    std::stringstream ss;
    ss<<"pozycja poszukiwacza "<<msg->latitude<<"  ,  "<<msg->longitude<<std::endl;
    pub_str.data=ss.str();
    //std::cout<<pub_str.data.c_str()<<std::endl;
   
    poszukiwacz_pozycaj= *msg;

}

void chatterCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
      
    std_msgs::String pub_str;
    std::stringstream ss;
    
    ss<<"lokalna pozycja scigajacego : "<<msg->pose.position.x<<"  ,  "<<msg->pose.position.y<<"  ,  "<<msg->pose.position.z<<std::endl;
    
    pub_str.data=ss.str();
    std::cout<<pub_str.data.c_str();
  

}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "kontrolalotow3");
    ros::NodeHandle nh;
    ros::NodeHandle n;
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State> ("uav1/mavros/state", 10, state_cb);
    
    ros::Subscriber pozycja_sciganego=n.subscribe<sensor_msgs::NavSatFix>("/uav0/mavros/global_position/global",1000,chatterCallback_scigany);

    ros::Subscriber pozycja_poszukiwacza=n.subscribe<sensor_msgs::NavSatFix>("/uav1/mavros/global_position/global",1000,chatterCallback_poszukiwacz);

    ros::Subscriber pozycja_poszukiwacza_local=n.subscribe<geometry_msgs::PoseStamped>("uav1/mavros/local_position/pose",1000,chatterCallback_poszukiwacz_local);

    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("uav1/mavros/setpoint_position/local", 1000);
    
    ros::Publisher statuser = n.advertise<mavros_msgs::State>("uav1/mavros/state", 10,state_cb);
    
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("uav1/mavros/set_mode");
      
   ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("uav1/mavros/cmd/arming");
        

   
    ros::Rate rate(20.0);

    ros::Time last_request = ros::Time::now();
    zadana.pose.position.x = 0;
    zadana.pose.position.y = 0;
    zadana.pose.position.z= 15;


    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(zadana);
        ros::spinOnce();
        rate.sleep();
    }

  


    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.base_mode = 0;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;
    
    while(ros::ok())
    {
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }
        //std::cout<<"nadaje"<<std::endl;
        local_pos_pub.publish(zadana);
        statuser.publish(status);
        ros::spinOnce();
        rate.sleep();
    }
        
        
         
      
    

    return 0;
}