#include<vector>
#include<GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

	enum CameraMove {
		forward,
		backward,
		left,
		right
	};

using namespace glm;
const GLfloat YAW = -90;
const GLfloat PITCH = -25;
const GLfloat SPEED = 6;
const GLfloat DISTANCE = 50;
const GLfloat angle = -90;

class Camera {
private:
	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 worldUp;

	GLfloat xPos;
	GLfloat yPos;
	GLfloat distance; 
	GLfloat Yaw;
	GLfloat pitch;
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat angle;

public:
	Camera(vec3 position = vec3(0, 0, 0), vec3 up = vec3(0, 1, 0),
		GLfloat Yaw = YAW, GLfloat Pitch = PITCH) :front(vec3(0, 0, -1)), movementSpeed(SPEED), mouseSensitivity(0.07), angle (180),distance(13)
	{
		this->position = position;
		this->worldUp = up;
		this->Yaw = Yaw;
		this->pitch = Pitch;
		this->updateCamera();
	}
	
	 mat4 getViewMatrix()
	{
		return  lookAt(position, position + front, up);
	}


	 float calculatorHoz()
	 {
		 return (float)(this->distance*cos(radians(pitch)));
	 }
	 float calculatorVer()
	 {
		 return (float)(this->distance*sin (radians(pitch)));
	 }
	 void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset,GLfloat movex,GLfloat movez, GLboolean constrainPitch = true)
	 {
		 xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;

		
		this->pitch += yOffset;
		 //d::cout << this->pitch;
		// if (this->pitch < 15) this->pitch = 15;
		 
		std::cout << this->Yaw<<" ";
		 angle += xOffset;
		 if (abs(angle > 360)) angle = 0;
		 this->position.x = movex+calculatorHoz()*sin(radians(angle));
		 this->position.z = (12+movez-calculatorHoz()*cos(radians(angle)));
			std::cout << angle<< std::endl;
		// this->position.y = //calculatorVer();
		 this->Yaw = -90+(angle-180) ;
			 ;
		 

		 if (constrainPitch)
		 {
			 if (this->pitch > 89.0f)
			 {
				 this->pitch = 89.0f;
			 }

			 if (this->pitch < -89.0f)
			 {
				 this->pitch = -89.0f;
			 }
		 }

		 // Update Front, Right and Up Vectors using the updated Eular angles
		 this->updateCamera();
	 }
	 void calPitch() {

	 }
	void HandleKeyBoard(CameraMove direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->movementSpeed*deltaTime;
		if (CameraMove::left == direction)
		{
			this->position -= this->right*velocity;
		} 
		if (CameraMove::right == direction)
		{
			this->position += this->right*velocity;
		}
		if (CameraMove::forward == direction)
		{
			this->position += this->front*vec3(velocity,0,velocity);
		}
		if (CameraMove::backward == direction)
		{
			this->position -= this->front*vec3(velocity, 0, velocity);
		} 
	}

	void updateCamera()
	{
		 vec3 front;
		front.x = cos( radians(this->Yaw))*cos( radians(this->pitch));
		front.y = sin( radians(this->pitch));
		front.z = sin( radians(this->Yaw))*cos( radians(this->pitch));

		this->front =  normalize(front);
		this->right =  normalize( cross(this->front, this->worldUp));
		this->up =  normalize( cross(this->right, this->front));
	}

};