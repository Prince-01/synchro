// synchro.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <boost\filesystem.hpp>
using namespace std;
using namespace boost::filesystem;

class dir {
	dir* _root;
	dir* _parent;
	path _directory;
	vector<dir> _directories;
	vector<path> _regularFiles;
public:
	dir(path directory, dir* parent) : _directory(directory), _parent(parent)
	{
		_root = this;
		if (!exists(directory))
			create_directory(directory);
		fillInDirsAndFiles();
	}
private:
	dir(path directory, dir* parent, dir* root) : dir(directory, parent)
	{
		_root = root;
	}
	void fillInDirsAndFiles()
	{
		for (auto&& p : directory_iterator(_directory))
		{
			if (is_directory(p))
				_directories.push_back(dir(p, this, _root));
			else if (is_regular_file(p))
				_regularFiles.push_back(p);
		}
	}
public:
	vector<dir>& getDirectories() {
		return _directories;
	}
	vector<path>& getRegularFiles() {
		return _regularFiles;
	}
	string getRelativePath() const {
		return getRelativePath(_directory);
	}
	string getRelativePath(path p) const
	{
		return p.string().substr(_root->_directory.string().length(), p.string().length() - _root->_directory.string().length());
	}
	string getRootPathDirectory() const {
		return _root->_directory.string();
	}
};

class synchro {
	dir _from;
	dir _to;
public:
	synchro(path from, path to) : _from(from, nullptr), _to(to, nullptr) {}
	void sync()
	{
		sync(_from, _to);
	}
private:
	void sync(dir& from, dir& to)
	{
		for (auto& f : from.getDirectories())
		{
			bool found = false;
			for (auto& t : to.getDirectories())
			{
				if (f.getRelativePath() == t.getRelativePath())
				{
					found = true;
					sync(f, t);
					break;
				}
			}
			if (!found)
				copyRecursively(f, to);
		}
		for (auto& f : from.getRegularFiles())
		{
			bool found = false;
			for (auto& t : to.getRegularFiles())
			{
				if (from.getRelativePath(f) == to.getRelativePath(t))
				{
					found = true;
					compareFilesAndPostNewVersionIfNeeded(f, t);
					break;
				}
			}
			if (!found)
				copy(f, to.getRootPathDirectory() + to.getRelativePath() + "\\" + f.filename().string());
		}
	}
	void copyRecursively(dir& what, dir& where) {
		path whereDir = path(where.getRootPathDirectory() + what.getRelativePath());
		create_directory(whereDir);

		for (auto& d : what.getDirectories())
			copyRecursively(d, where);
		for (auto& f : what.getRegularFiles())
			copy(f, whereDir.string() + "\\" + f.filename().string());
	}
	void compareFilesAndPostNewVersionIfNeeded(path& what, path& where) {

	}
};

int main()
{
	//dir from(path("D:\\b-up"), nullptr);
	synchro s(path("C:\\totalcmd"), path("C:\\Users\\Prince\\Documents\\totalcmd2"));
	s.sync();
	return 0;
}

