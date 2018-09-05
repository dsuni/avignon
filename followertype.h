#ifndef FOLLOWERTYPE_H
#define FOLLOWERTYPE_H
#include <map>
#include <string>

using namespace std;

enum FollowerType { normal, big, builder, pig,
										followerTypeFirst = normal, followerTypeLast = pig };

struct followerTypeMap : public map<FollowerType, string> {
	followerTypeMap() {
		this->operator[](normal) = "Regular";
		this->operator[](big) = "Big";
		this->operator[](builder) = "Builder";
		this->operator[](pig) = "Pig";
	};
};
#endif
