#include <utility>

//
// Created by Christoffer Wärnbring on 2019-03-10.
//
#include <string>
std::string ReplaceAll(std::string Original, const std::string ToReplace, const std::string ReplaceWith) {
	std::string Copy = std::move(Original);
	auto pos = Copy.find(ToReplace);
	while (Copy.size() > pos) {
		Copy.replace(pos, ToReplace.size(), ReplaceWith);
		pos = Copy.find(ToReplace,pos+1);
	}
	return Copy;
}
