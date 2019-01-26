#include "../../Window.h"
#include "../GameObject.h"

enum Fatness
{
	LOW = 3,
	MEDIUM = 2,
	HIGH = 1
};

class HomePlayer : public GameObject
{
public:

	Fatness fatness;
	int hp;
	float initialYcoord;

	float boundingWidth;
	float boundingHeight;

	bool jumping;
	float lastJumpingTime;
	bool beingHit;
	float lastHitTime;


	void processCommands(Window& window, float dt, float t, Shader& shader)
	{
		float speedMultiplier = velocityFactor() * dt;

		if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = -glm::vec3{ dt ,0.0f,0.0f };
			transform.position += toAdd;
			shader.bind();
			shader.setUniformValue("animation", 1);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
		}
		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			
			glm::vec3 toAdd = glm::vec3{ dt,0.0f,0.0f };
			transform.position += toAdd;
			shader.bind();
			shader.setUniformValue("animation", 0);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
		}
		if (isKeyPressed(GLFW_KEY_SPACE, window.getGLFWwindow()))
		{
			if (!jumping)
			{
				jumping = true;
				lastJumpingTime = t;
//				advanceJump(t);
	//			shader.bind();
	//			shader.setUniformValue("animation", 0);
	//			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			}
		}

	}

	void advanceJump(float t)
	{
		float jumpDuration = 1.0f;
		transform.position.y = initialYcoord -4.0f * jumpHeight() / ((jumpDuration)*(jumpDuration)) * (t - lastJumpingTime) * (t - (lastJumpingTime + jumpDuration) );
		if (t >= lastJumpingTime + jumpDuration)
		{
			jumping = false;
			// restore initial y position
		}
	}

	void updateHitStatus(float t)
	{
		float hitDuration = 2.0;
		if (t >= hitDuration + lastHitTime)
		{
			beingHit = false;
			
		}
	}

	bool AABBcollision(float x, float y, float width, float height)
	{
		if (
			transform.position.x < x + width &&
			transform.position.x + this->boundingWidth > x &&
			transform.position.y < y + height &&
			transform.position.y + this->boundingHeight > y
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	float velocityFactor()
	{
		return 10.0f * fatness;
	}

	float jumpHeight()
	{
		return 1.0f * fatness;
	}

};


namespace home
{
	class Enemy : public GameObject
	{
	public:
		void advance(float t, float dt, float speed)
		{
			transform.position.x -= speed * dt;
		}

		void jump(float t, float dt)
		{

		}

	};
}