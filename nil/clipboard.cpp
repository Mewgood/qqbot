#include <nil/clipboard.hpp>

#include <cstdlib>

#include <nil/windows.hpp>

#ifdef NIL_WINDOWS

namespace nil
{
	struct system_clipboard
	{
		system_clipboard()
		{
			//if(::OpenClipboard(0) == 0)
			if(::OpenClipboard(::GetForegroundWindow()) == 0)
				throw exception("Unable to open clipboard");
		}

		~system_clipboard()
		{
			::CloseClipboard();
		}
	};

	std::string get_clipboard()
	{
		system_clipboard clipboard;
		::HANDLE clipboard_handle = ::GetClipboardData(CF_TEXT);
		std::string output(reinterpret_cast<char *>(::GlobalLock(clipboard_handle)));
		::GlobalUnlock(clipboard_handle);
		return output;
	}

	void set_clipboard(std::string const & input)
	{
		system_clipboard clipboard;
		std::size_t c_string_size = input.length() + 1;
		::HGLOBAL allocation = ::GlobalAlloc(GMEM_DDESHARE, c_string_size);
		std::memcpy(::GlobalLock(allocation), input.c_str(), c_string_size);
		::GlobalUnlock(allocation);
		::EmptyClipboard();
		::SetClipboardData(CF_TEXT, allocation);
	}
}

#endif

