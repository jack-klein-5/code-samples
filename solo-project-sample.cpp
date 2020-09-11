/**************************************************************************

This sample is a part of my personal project I've worked on more recently.
This project required me to work with a bunch of systems I hadn't had much
experience in before, and this is an example of that. Typically I don't
do much graphics work, but because I'm the sole developer I had to figure
out how to make sprites appear on the screen in a way that made sense with
other systems. This might not be the final version of Sprite code in
whatever Project Ice Cream becomes, I'm not finished with it yet, but this
current code works and does what I need it to do.

**************************************************************************/


/**************************************************************************/
/*!
\file   Sprite.cpp
\author Jack Klein
\par    email: klein.jack5@gmail.com
\date   05/12/20
\brief  This file contains the definitions for the Sprite class
        Sprite just has color internally, and grabs Transform and Textures
*/
/**************************************************************************/

#include "stdafx.hpp"
#include "GameObject/GameObject.hpp"
#include "Component/Sprite.hpp"
#include "Component/Transform.hpp"
#include "Component/Texture.hpp"
#include "Component/Animation.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Shaders.hpp"


/**********************************************************************/
/*!
  \brief
    Clone an existing sprite component
  \param newParentPtr
    The new parent to assign to the newly created clone
  \return 
    The newly created clone
*/
/**********************************************************************/
std::shared_ptr<Component> Sprite::Clone(std::weak_ptr<GameObject> newParentPtr)
{
  /* Allocate a new sprite with parent pointer */
  std::shared_ptr<Sprite> newSprite(std::make_shared<Sprite>(newParentPtr));
  /* Copy the color */
  newSprite->SetColor(color_);

  /* Add sprite to sprite list */
  Graphics::AddSprite(newSprite);

  return newSprite;
}

/**********************************************************************/
/*!
  \brief
    Render this sprite component to the screen
*/
/**********************************************************************/
void Sprite::Render()
{
  /* Check if we have a transform component */
  std::shared_ptr<GameObject> parent = GetParent().lock();
  std::shared_ptr<Component> component = parent->GetComponent("Transform", false);
  if (!component.get())
  {
    /* If we don't have a transform just make one */
    std::shared_ptr<Transform> newTransform(std::make_shared<Transform>(parent));
    std::shared_ptr<Component> newComponent = std::dynamic_pointer_cast<Component>(newTransform);
    parent->AddComponent("Transform", newComponent);
    component = newComponent;
  }
  assertm(component.get(), "Transform wasn't created for some reason");
  std::shared_ptr<Transform> transform = std::dynamic_pointer_cast<Transform>(component);
  glm::mat4 matrix = transform->GetMatrix();

  /* Use our shader */
  Graphics::UseShader();
  
  /* Optionally get a texture component */
  component = parent->GetComponent("Texture", false);
  if (!component.get())
  {
    /* We don't have a texture, use default texture */
    std::shared_ptr<Texture> newTexture(std::make_shared<Texture>(parent));
    std::shared_ptr<Component> newComponent = std::dynamic_pointer_cast<Component>(newTexture);
    parent->AddComponent("Texture", newTexture);
    component = newComponent; 
  }

  /* Get the shader program handle */
  int shaderHandle = Shaders::GetProgramHandle();

  /* We should now have a texture, so bind it */
  std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(component);
  glActiveTexture(GL_TEXTURE0);
  unsigned texHandle = texture->GetTextureHandle();
  glBindTexture(GL_TEXTURE_2D, texHandle);
  
  /* Set texture uniforms */
  glUniform1i(glGetUniformLocation(texHandle, "textureImage"), 0);
  glm::vec2 texDim = texture->GetTexDimensions();
  glUniform2f(glGetUniformLocation(shaderHandle, "textureDim"), texDim.x, texDim.y);

  /* Optionally get an animation component */
  component = parent->GetComponent("Animation", false);
  if (!component.get())
  {
    /* We don't have an animation, use default values */
    glUniform2f(glGetUniformLocation(shaderHandle, "animationOffset"), 0, 0);
  }
  else
  {
    /* We have an animation, set animation uniform */
    std::shared_ptr<Animation> animation = std::dynamic_pointer_cast<Animation>(component);
    glm::vec2 aniOffset = animation->GetAnimationOffset();
    glUniform2f(glGetUniformLocation(shaderHandle, "animationOffset"), aniOffset.x, aniOffset.y);
  }
  

  /* Set up VAO to draw */
  glBindVertexArray(Graphics::GetMeshVaoID());

  /* Set color uniform */
  int vertexColorLocation = glGetUniformLocation(shaderHandle, "color");
  assertm(vertexColorLocation != -1, "Couldn't find color uniform");
  glUniform4f(vertexColorLocation, color_.x, color_.y, color_.z, color_.w);

  /* Set matrix uniform */
  matrix = Graphics::GetWorld2NDC() * matrix;
  int instanceMatrixLocation = glGetUniformLocation(shaderHandle, "instanceMatrix");
  assertm(instanceMatrixLocation != -1, "Couldn't find matrix uniform");
  glUniformMatrix4fv(instanceMatrixLocation, 1, GL_FALSE, (GLfloat*)(&matrix[0][0]));

  /* Draw every rectangle */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  /* Unbind textures */
  glBindTexture(GL_TEXTURE_2D, 0);
}
