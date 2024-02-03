#include <iostream>

#include "compat.h"
#include "lib/os.h"


void print_directory_item(const CratOS::Filesystem::DirectoryItem& di)
{
	std::cout << di.name << " (" << (di.type == di.FILE ? "FILE)" : "FOLDER)") << '\n';
}

std::vector<CratOS::Filesystem::DirectoryItem> print_dir(const CratOS::Filesystem& os, const std::vector<CratOS::Filesystem::DirectoryItem>& dir)
{
	for (auto& d : dir)
	{
		print_directory_item(d);
	}
	std::cout << std::endl;
	std::cout << std::endl;

	return dir;
}


std::vector<CratOS::Filesystem::DirectoryItem> print_current_dir(const CratOS::Filesystem& os)
{
	std::vector<CratOS::Filesystem::DirectoryItem> dir = os.ListDirectory(os.GetCurrentDirectory());

	return print_dir(os, dir);
}

std::string print_file(CratOS::Filesystem& os, const std::string& filename)
{
	std::span<char> fileContent = os.ReadFile(filename);
	std::string content{ fileContent.begin(), fileContent.end() };
	std::cout << content << std::endl;

	return content;
}

void explore_directory(CratOS::Filesystem& os, const std::string& dir)
{
	std::cout << "\n---\nExploring " << dir << '\n';
	auto dirs = os.ListDirectory(dir);
	print_dir(os, dirs);

	if (dirs.empty())
	{
		return;
	}

	for (CratOS::Filesystem::DirectoryItem f : dirs)
	{
		if (f.type == f.FILE)
		{
			std::string fileContent = print_file(os, f.name);
			if (fileContent.find("flag") != std::string::npos)
			{
				std::cout << "FLAG FOUND!" << std::endl;
				return;
			}
		}
		if (f.type == f.FOLDER)
		{
			explore_directory(os, f.name);
		}
	}
}

int main()
{
	CratOS::Filesystem os{ "puzzle.fs" };

	try
	{
		//// Get the current directory
		print_current_dir(os);

		os.ChangeDirectory("aa");
		os.ChangeDirectory("bb");
		os.ChangeDirectory("d");
		print_current_dir(os);

		print_file(os, "not_a_flag.exe");

		//explore_directory(os, os.GetCurrentDirectory());
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}