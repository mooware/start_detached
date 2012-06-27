#include <stdio.h>
#include <string.h>

#include <Windows.h>

static const char DOUBLE_QUOTE = '"';
static const char BACKSLASH = '\\';

// escapes arg so that will be parsed as a single command line argument
char *escape_arg(const char *arg);

// returns the windows error text for the given code
char *get_error_text(int errcode);

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("usage: %s [application] [arguments...]\n", argv[0]);
    return 1;
  }

  // escape all arguments
  char *escaped_arguments = 0;
  size_t arguments_len = 0;
  for (int i = 1; i < argc; ++i)
  {
    char *arg = escape_arg(argv[i]);
    arguments_len += (strlen(arg) + 1); // + 1 for the blanks between args
  
    escaped_arguments = (char *) realloc(escaped_arguments, arguments_len + 1);

    if (i == 1)
    {
      memcpy(escaped_arguments, arg, arguments_len);
    }
    else
    {
      strcat(escaped_arguments, " ");
      strcat(escaped_arguments, arg);
    }
  
    free(arg);
  }

  // prepare the process
  STARTUPINFO startup_info;
  PROCESS_INFORMATION process_info;

  ZeroMemory(&startup_info, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  ZeroMemory(&process_info, sizeof(process_info));

  // configure to detach
  DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;

  // create the process
  BOOL success = CreateProcess(NULL, escaped_arguments,
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   dwCreationFlags,
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &startup_info,
                   &process_info);

  if (success)
  {
    printf("created new process %d\n", process_info.dwProcessId);
  }
  else
  {
    int errcode = GetLastError();
    printf("failed to create process\nerror code %d: %s\n", errcode, get_error_text(errcode));
  }

  return (success ? 0 : 1);
}

//------------------------------------------------------------------------------

char *escape_arg(const char *arg)
{
  // double length is always enough, + 2 for leading and trailing quotes
  size_t oldlen = strlen(arg);
  size_t newlen = (oldlen * 2) + 2;

  char *newarg = (char *) malloc(newlen + 1);

  // the first quote can already be set, but the second would be escaped
  newarg[0] = DOUBLE_QUOTE;
  memcpy(newarg + 1, arg, oldlen + 1);

  char *it = newarg + 1;
  while (*it != '\0')
  {
    if (*it == DOUBLE_QUOTE || *it == BACKSLASH)
    {
      memmove(it + 1, it, strlen(it) + 1);
      *it = BACKSLASH;
      ++it;
    }

    ++it;
  }

  char *end = newarg + strlen(newarg);
  end[0] = DOUBLE_QUOTE;
  end[1] = '\0';

  return newarg;
}

//------------------------------------------------------------------------------

char *get_error_text(int errcode)
{
  char *buffer = 0;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, // The location of the message definition (not used)
                errcode,
                0, // language identifier (default)
                (LPTSTR) &buffer, // buffer to write into (will be allocated)
                0, // size of the buffer
                NULL); // values to fill placeholders

  // not sure how buffer is allocated, because it has to be deleted with LocalFree.
  // to be safe, make a malloc'd copy.
  char *result = strdup(buffer);
  LocalFree(buffer);

  return result;
}