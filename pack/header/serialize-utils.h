#ifndef _SERIALIZE_GROUNDNUT_UTILS_H_
#define _SERIALIZE_GROUNDNUT_UTILS_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include "boost-serialize.h"

namespace groundnut{



template <class T>
inline void Serialize(const T& obj, std::filesystem::path outFile)
{
	std::filesystem::create_directories(outFile.parent_path());

	std::ofstream ofs(outFile.string(), std::ios::binary | std::ios::out);
	std::ofstream oftext(outFile.string() + ".txt",  std::ios::out);

	if (ofs.is_open() && oftext.is_open())
	{
		typename boost::archive::binary_oarchive oa(ofs);
		typename boost::archive::text_oarchive otext(oftext);

		oa << obj;
		otext << obj;
	}
	else
	{
        throw "serialize error.";
	}
}

template<class T>
inline void LoadBin(T& obj, std::filesystem::path inputFile)
{
	std::ifstream ifs(inputFile.string(), std::ios::binary | std::ios::in);

	if (ifs.is_open())
	{
		typename boost::archive::binary_iarchive ia(ifs);
		ia >> obj;
	}
	else
	{
		throw "load bin error.";
	}
}
}

#endif
