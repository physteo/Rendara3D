#include "../../Window.h"
#include "../GameObject.h"
#include "LevelState.h"
#include "irrKlang.h"

namespace home {

	class Tomato
	{
	public:
		const float boundingHeight = 0.5f;
		const float boundingWidth  = 0.5f;
		Transform transform;

	};

	class Doe : public Transform
	{
	public:
		const float boundingHeight = 0.5f;
		const float boundingWidth = 0.5f;
		Transform transform;
	};

	class Inventory
	{
		const glm::vec3 inventoryPosition = glm::vec3{+3.0f, +2.0f, 0.02f};
		const glm::vec3 inventoryPositionDelta = glm::vec3{ +0.75f, 0.0f, 0.0f };
		const glm::vec3 inventoryHpPosition = glm::vec3{ -4.0f, +2.0f, 0.002f };
		const glm::vec3 inventoryHpPositionDelta = glm::vec3{ 0.4, +0.0f, 0.0f };


		const int maxHp = 5;
		const float inventoryScale = 0.5f;
	public:
		int doeCounter;
		int tomatoCounter;
		int hp;
		Inventory() : doeCounter{ 0 }, tomatoCounter{ 0 }, hp{ maxHp } {}

		void draw(Shader& shader, Mesh& mesh, Material& material)
		{
			shader.bind();
			shader.setUniformValue("x", -(doeCounter!=0) + 1);
			shader.setUniformValue("y", 7);
			mesh.draw(inventoryScale, inventoryPosition, glm::vec3{ 90.0f, 0.0f, 0.0f }, shader, material);

			shader.bind();
			shader.setUniformValue("x", -(tomatoCounter != 0) + 3);
			shader.setUniformValue("y", 7);
			mesh.draw(inventoryScale, inventoryPosition + inventoryPositionDelta, glm::vec3{ 90.0f, 0.0f, 0.0f }, shader, material);

			for (int i = 0; i < maxHp; i++)
			{
				shader.bind();
				shader.setUniformValue("x", -(hp > i) + 1);
				shader.setUniformValue("y", 6);
				mesh.draw(inventoryScale, inventoryHpPosition + (float)i * inventoryHpPositionDelta, glm::vec3{ 90.0f, 0.0f, 0.0f }, shader, material);
			}

		}
	};
}


enum Fatness
{
	LOW = 3,
	MEDIUM = 2,
	HIGH = 1
};

class HomePlayer : public GameObject
{

public:
	home::Inventory inventory;
	Fatness fatness;


	float initialYcoord;
	float initialZcoord;

	float boundingWidth;
	float boundingHeight;

	bool jumping;
	float lastJumpingTime;
	bool beingHit;
	float lastHitTime;
	bool moving;

	float lastChangeFat;
	const float fatDuration = 20.0f;

	void processCommands(Window& window, float dt, float t, Shader& shader, LevelState levelState, irrklang::ISoundEngine* SoundEngine)
	{
		float speedMultiplier = velocityFactor();

		if (isKeyPressed(GLFW_KEY_1, window.getGLFWwindow()))
		{
			fatness = HIGH;
			shader.bind();
			shader.setUniformValue("animation", (fatness-1) * 2);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			boundingWidth  = 0.5f;
			boundingHeight = 0.5f;
		}
		if (isKeyPressed(GLFW_KEY_2, window.getGLFWwindow()))
		{
			fatness = MEDIUM;
			shader.bind();
			shader.setUniformValue("animation", (fatness - 1) * 2);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			boundingWidth = 0.4f;
			boundingHeight = 0.5f;
		}
		if (isKeyPressed(GLFW_KEY_3, window.getGLFWwindow()))
		{
			fatness = LOW;
			shader.bind();
			shader.setUniformValue("animation", (fatness - 1) * 2);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			boundingWidth = 0.3f;
			boundingHeight = 0.5f;
		}

		if ( isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = -glm::vec3{ dt ,0.0f,0.0f } * speedMultiplier;
			transform.position += toAdd;
			shader.bind();
			shader.setUniformValue("animation", (fatness - 1) * 2 + 1);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			moving = true;
		}
		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			
			glm::vec3 toAdd = glm::vec3{ dt,0.0f,0.0f } * speedMultiplier;
			transform.position += toAdd;
			shader.bind();
			shader.setUniformValue("animation", (fatness - 1) * 2 + 0);
			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			moving = true;

		}
		if (isKeyPressed(GLFW_KEY_SPACE, window.getGLFWwindow()))
		{

			if (!jumping)
			{
				SoundEngine->play2D("./res/sounds/jump1.wav", GL_FALSE);
				jumping = true;
				lastJumpingTime = t;
//				advanceJump(t);
	//			shader.bind();
	//			shader.setUniformValue("animation", 0);
	//			shader.setUniformValue("t", (int)(t / 0.25f) % 4);
			}
		}

		if (!isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()) && !isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			moving = false;
		}

		if (levelState == LevelState::AT_HOME)
		{
			if (isKeyPressed(GLFW_KEY_DOWN, window.getGLFWwindow()))
			{
				// can move up and down
				glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,dt } *speedMultiplier;
				transform.position += toAdd;
				shader.bind();
				shader.setUniformValue("animation", (fatness - 1) * 2 + 0);
				shader.setUniformValue("t", (int)(t / 0.25f) % 4);
				moving = true;
			}
			if (isKeyPressed(GLFW_KEY_UP, window.getGLFWwindow()))
			{
				// can move up and down
				glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,dt } *speedMultiplier;
				transform.position -= toAdd;
				shader.bind();
				shader.setUniformValue("animation", (fatness - 1) * 2 + 0);
				shader.setUniformValue("t", (int)(t / 0.25f) % 4);
				moving = true;
			}

		}

	}

	bool advanceJump(float t)
	{
		float jumpDuration = 1.0f;
		transform.position.y = initialYcoord -4.0f * jumpHeight() / ((jumpDuration)*(jumpDuration)) * (t - lastJumpingTime) * (t - (lastJumpingTime + jumpDuration) );
		if (t >= lastJumpingTime + jumpDuration)
		{
			jumping = false;
			// restore initial y position
			transform.position.y = initialYcoord;
			return false;
		}
		return true;
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
		
		//transform.position.x < x + width/2.0 &&
		//transform.position.x + this->boundingWidth/2.0 > x &&
		//transform.position.y < y + height/2.0 &&
		//transform.position.y + this->boundingHeight/2.0 > y
		// Collision x-axis?

		bool collisionX = transform.position.x + boundingWidth >= x &&
			x + width >= transform.position.x;
		// Collision y-axis?
		bool collisionY = transform.position.y + boundingHeight >= y &&
		y + height >= transform.position.y;
		// Collision only if on both axes
		return collisionX && collisionY;

	}
	bool AABBcollisionXZ(float x, float z, float width, float height)
	{

		//transform.position.x < x + width/2.0 &&
		//transform.position.x + this->boundingWidth/2.0 > x &&
		//transform.position.y < y + height/2.0 &&
		//transform.position.y + this->boundingHeight/2.0 > y
		// Collision x-axis?

		bool collisionX = transform.position.x + boundingWidth >= x &&
			x + width >= transform.position.x;
		// Collision y-axis?
		bool collisionZ = transform.position.z + boundingHeight >= z &&
			z + height >= transform.position.z;
		// Collision only if on both axes
		return collisionX && collisionZ;

	}
private:
	float velocityFactor()
	{
		return 1.0f * fatness;
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
		float tau = 15.0f;
		float spawnedTime;

		int enemyID;
		float boundingWidth;
		float boundingHeight;

		void advance(float t, float dt, float speed)
		{
			transform.position.x -= speed * dt;
			transform.rotation.z += 360.0f * dt;
		}

		void jump(float t, float dt)
		{

		}

	};


	class Grass : public Transform
	{
	public:
		float boundingWidth;
		float boundingHeight;

		bool moving;
		float lastTimeMoving;

		void updateStates(float t)
		{
			float moveDuration = 0.2f;
			if (t - lastTimeMoving > 0.2f)
				moving = false;
		}
	};
}