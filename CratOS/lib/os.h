#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>

namespace CratOS
{
	class Filesystem
	{
		struct Impl;
		std::unique_ptr<Impl> pImpl;

	public:
		struct Exception : std::exception
		{
		};

		struct InvalidPathException : Exception
		{
			char const* what() const override;
		};

		struct NotAFolderException : Exception
		{
			char const* what() const override;
		};

		struct NotAFileException : Exception
		{
			char const* what() const override;
		};

		struct PathNotFoundException : Exception
		{
			char const* what() const override;
		};


		struct DirectoryItem
		{
			std::string name;
			enum Type
			{
				FOLDER, FILE
			} type;
		};

		Filesystem(const std::string& filenamePath);
		~Filesystem();

		void ChangeDirectory(const std::string& path);
		std::span<char> ReadFile(const std::string& path);
		std::vector<DirectoryItem> ListDirectory(const std::string& path) const;
		std::string GetCurrentDirectory() const;

		
	};
}
