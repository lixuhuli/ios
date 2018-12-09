////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _IDOWNLOAD_INFO_INCLUDE_H_
#define _IDOWNLOAD_INFO_INCLUDE_H_

namespace download {
    class iDownloadInfo
        : public base::SupportsWeakPtr<iDownloadInfo>
        , public base::RefCountedThreadSafe<iDownloadInfo> {
    public:
        virtual ~iDownloadInfo() {}

        virtual CHttpDownload* http() = 0;
        virtual const wstring& save_path() = 0;
        virtual const string& url() = 0;
        virtual const __int64& total_size() = 0;
        virtual const __int64& load_size() = 0;
        virtual const __int64& last_time() = 0;
        virtual const __int64& size() = 0;
        virtual const __int64& date() = 0;
        virtual const int& speed() = 0;
    };
}


#endif  // !#define (_IDOWNLOAD_INFO_INCLUDE_H_)
