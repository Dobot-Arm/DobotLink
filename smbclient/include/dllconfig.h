
#ifndef __smb2_dllconfig_h__
#define __smb2_dllconfig_h__

#if defined(_WIN32) || defined(_WIN64)
#  if defined(SMB2_SHARED_LIB_SYMBOL)
#  define SMB2_EXPORT_SYMBOL __declspec(dllexport)
#  else
#  define SMB2_EXPORT_SYMBOL
#  endif
#else
#  if defined(SMB2_SHARED_LIB_SYMBOL)
#  define SMB2_EXPORT_SYMBOL __attribute__ ((visibility("default")))
#  else
#  define SMB2_EXPORT_SYMBOL
#  endif
#endif

#endif /* __smb2_dllconfig_h__ */
