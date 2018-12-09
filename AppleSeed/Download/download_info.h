////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 The Sun.AC Authors . All Rights Reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DOWNLOAD_INFO_INCLUDE_H_
#define _DOWNLOAD_INFO_INCLUDE_H_

#include "i_download_info.h"

namespace download {
    class DownloadInfo
        : public iDownloadInfo {
    public:
        explicit DownloadInfo()
         : nDate_(0), nTotalSize_(0), nLoadSize_(0)
         , nLastTime_(0), nSize_(0), nSpeed_(0), http_(nullptr) {
             http_ = new CHttpDownload();
        }
        virtual ~DownloadInfo() {
            clear();
        }

        DownloadInfo(DownloadInfo& info) {
            strUrl_ = info.strUrl_;
            strSavePath_ = info.strSavePath_;
            nDate_ = info.nDate_;
            nTotalSize_ = info.nTotalSize_;
            nLoadSize_ = info.nLoadSize_;
            nLastTime_ = info.nLastTime_;
            nSize_ = info.nSize_;
            nSpeed_ = info.nSpeed_;
            http_ = info.http_;
        }

    public:
        scoped_refptr<DownloadInfo> cloner() {
            return internal_cloner();
        }

        scoped_refptr<DownloadInfo> internal_cloner() {
            return new DownloadInfo(*this);
        }

        void clear() {
            strUrl_ = "";
            strSavePath_ = L"";
            nDate_ = 0;
            nTotalSize_ = 0;
            nLoadSize_ = 0;
            nLastTime_ = 0;
            nSize_ = 0;
            nSpeed_ = 0;
            if (http_) delete http_;
            http_ = nullptr;
        }

        virtual CHttpDownload* http() override { return http_; }
        void set_http(CHttpDownload* http) { http_ = http; }

        virtual const wstring& save_path() override { return strSavePath_; }
        void set_save_path(const wstring& save_path) { strSavePath_ = save_path; }

        virtual const string& url() override { return strUrl_; }
        void set_url(const string& url) { strUrl_ = url; }

        virtual const __int64& total_size() override { return nTotalSize_; }
        void set_total_size(const __int64& size) { nTotalSize_ = size; }

        virtual const __int64& load_size() override { return nLoadSize_; }
        void set_load_size(const __int64& size) { nLoadSize_ = size; }

        virtual const __int64& last_time() override { return nLastTime_; }
        void set_last_time(const __int64& nLastTime) { nLastTime_ = nLastTime; }

        virtual const __int64& size() override { return nSize_; }
        void set_size(const __int64& size) { nSize_ = size; }

        virtual const int& speed() override { return nSpeed_; }
        void set_speed(const int& speed) { nSpeed_ = speed; }

        virtual const __int64& date() override { return nDate_; }
        void set_date(const __int64& date) { nDate_ = date; }

    private:
        string strUrl_;			    // 下载链接地址
        wstring strSavePath_;	    // 下载保存路径
        __int64 nDate_;			    // 下载完成时间
        __int64 nTotalSize_;		// 下载项大小
        __int64 nLoadSize_;		    // 已下载大小
        __int64 nLastTime_;		    // 时间戳
        __int64 nSize_;
        int nSpeed_;			    // 下载速度 单位 KB/S
        CHttpDownload* http_;		// 下载库指针
    };
}

#endif  // !#define (_DOWNLOAD_INFO_INCLUDE_H_)
