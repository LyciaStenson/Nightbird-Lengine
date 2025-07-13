#pragma once

#define REGISTER_IS_CUSTOM_OBJECT() \
	bool IsCustomObject() const override { return true; }