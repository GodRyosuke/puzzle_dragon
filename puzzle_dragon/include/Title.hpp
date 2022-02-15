#pragma once

#include <stdlib.h>
#include <iostream>
#include"Scene.hpp"
#include "CommonData.hpp"

class Title : public Scene {
public:
	Title(CommonData* const commonData);
	virtual ~Title();
	virtual Scene* update() override;
	virtual void draw() override;
	virtual void input() override;

};

template<>
Scene* Scene::makeScene<Title>() {
	return new Title(mCommonData);
}