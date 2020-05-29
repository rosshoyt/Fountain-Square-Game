#pragma once
#include "GameObject.h"

class InstancedObject {
public:
	/*struct InstanceData {
		glm::vec3 trans, scale, rotAngs;
		bool destroyed = false;
	};*/

	InstancedObject(const char* filepath, Shader* shader, int numInstances) : model(filepath), shader(shader), numInstances(numInstances) {
		rotAngs = new float[numInstances];
	}


	
	virtual void drawInstances(glm::mat4 projection, glm::mat4 view) {}//= 0;

	virtual void deactivateInstance(unsigned int instanceNumber) {}

protected:
	Shader* shader;
	Model model;

	unsigned int numInstances;
	glm::mat4* modelMatrices;// size = numInstances
	float* rotAngs; // array holding the rotation (euler) angles of the instances. size = numInstances

	virtual void initModelTransformations() {}// = 0;

	//virtual void configureInstancedArray() {} //= 0;
	void configureInstancedArray() {
		// configure instanced array
		// -------------------------
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		// set transformation matrices as an instance vertex attribute (with divisor 1)
		// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
		// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
		// -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < model.meshes.size(); i++)
		{
			unsigned int VAO = model.meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
	
private:

};