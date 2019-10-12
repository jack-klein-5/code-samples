/**************************************************************************

This sample is a part of a much bigger game project I did during sophomore
year at DigiPen. I chose a sample with a very specific purpose that didn't
have too many dependencies on other things and is pretty modular. The 
purpose of this function was to get the ability to take objects in the
game and drag them around the screen with the mouse. With this, the game
designers and artists would be able to easily move assets using my editor
without having to deal with complicated coordinates or any guesswork.

**************************************************************************/


//------------------------------------------------------------------------------
//
// File Name:	DragNDrop.cpp
// Author(s):	Jack Klein (jack.klein)
// Project:		Blackout
// Course:		GAM200
//
// Copyright © 2019 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

void EnableDragNDrop(Serializer & serializer)
{
  static GameObject *candidate = nullptr;

  /* If the mouse is clicked, find the object being clicked on */
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
  {
    /* K, I'm going to loop through all the objects in the serializer list, and find the first object whose x and y +- size of object / 2 bounds cross my cursor. 
       The candidate will be whichever object has its center closest to the cursor. Once a candidate is found, I'll move the object center to the center of the mouse cursor. */

    /* Get the list of objects from serializer */
    std::vector<std::string> objectList = serializer.GetObjectList();

    /* For each object determine if it's a candidate using mousePos */
    /* Only find candidate if one isn't selected */
    if (candidate == nullptr)
    {
      for (int i = 0; i < (signed)(objectList.size()); ++i)
      {
        GameObject * testObject = &GameObjectManagerFindObject(objectList[i]);

        /* Get the object's tranform, if no transform exists, move on to next object */
        TransformPtr objectTransform = std::dynamic_pointer_cast<Transform>(testObject->Get(Component::ComponentTypes::ctTransform));
        if (!objectTransform)
        {
          continue;
        }

        /* First case, find the first object cursor is crossing */
        if (candidate == nullptr)
        {
          /* Get the max and mix x and y values of test object */
          float leftEdge = objectTransform->GetTranslation().x - (objectTransform->GetScale().x / 2);
          float rightEdge = objectTransform->GetTranslation().x + (objectTransform->GetScale().x / 2);
          float topEdge = objectTransform->GetTranslation().y + (objectTransform->GetScale().y / 2);
          float bottomEdge = objectTransform->GetTranslation().y - (objectTransform->GetScale().y / 2);

          /* If the cursor is within the edge bounds, make it the candidate */
          if (leftEdge < mousePos.x && rightEdge > mousePos.x && topEdge > mousePos.y && bottomEdge < mousePos.y)
          {
            candidate = testObject;
          }
        }

        /* After first case, change candidate if new test object's center is closer to cursor than current candidate */
        else
        {
          TransformPtr candidateTransform = std::dynamic_pointer_cast<Transform>(candidate->Get(Component::ComponentTypes::ctTransform));

          if (abs(objectTransform->GetTranslation().x - mousePos.x) < abs(candidateTransform->GetTranslation().x - mousePos.x)
            && abs(objectTransform->GetTranslation().y - mousePos.y) < abs(candidateTransform->GetTranslation().y - mousePos.y))
          {
            candidate = testObject;
          }
        }
      }
    }
    if (candidate)
    {
      /* Candidate has been found, move candidate center to mousePos */
      TransformPtr candidateTransform = std::dynamic_pointer_cast<Transform>(candidate->Get(Component::ComponentTypes::ctTransform));
      candidateTransform->SetTranslation(glm::vec3(mousePos.x, mousePos.y, candidateTransform->GetTranslation().z));
    }
  }
  else
  {
    candidate = nullptr;
  }
}