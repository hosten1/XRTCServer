#include "server/tcp_connection.h"
#include <cstring>
#include <unistd.h> // 包含 close 函数的头文件
#include <algorithm> // 包含 std::max 函数的头文件

#include "base/socket.h"
#include "tcp_connection.h"
#include "rtc_base/logging.h"

namespace lrtc
{
    TcpConnection::TcpConnection(int fd, const char *ip, int port) : fd_(fd), port_(port) /*,queryBuf_(sdsempty)*/
    {
        memset(ip_, 0, sizeof(ip_));
        if (ip)
        {
            strncpy(ip_, ip, sizeof(ip_) - 1);
        }
    }

    TcpConnection::TcpConnection(int fd) : fd_(fd)
    {
        sock_peet_to_string(fd_, ip_, &port_);
        RTC_LOG(LS_INFO) << "TcpConnection::TcpConnection fd:" << fd << " ip:" << ip_ << " port:" << port_;
    }

    TcpConnection::~TcpConnection()
    {
#ifdef USE_SDS
        sdsfree(queryBuf_);
#endif
    }

    int TcpConnection::read(int fd, 
                            std::function<void( Json::Value, uint32_t)> callback)
    {
        // 本次实际上读取的大小
        // int nread = 0;
        // const int read_len =  bytes_expected_;
        // int ensure_len = std::max((int)read_len, (int)(read_len + queryBuf_.size()));
        // queryBuf_.EnsureCapacity(ensure_len);
        // nread = sock_read_data(fd, queryBuf_.data() + read_len, read_len);
        //    read_len;
        // int size_r = queryBuf_.size() + read_len;
        // queryBuf_.SetSize(size_r);
        int nread = 0;
        const int read_len = bytes_expected_;
        RTC_LOG(LS_INFO) << "TcpConnection::read() begin. read_len:"<< read_len
                         <<", fd:" << fd << " ip:" << ip_ << " port:" << port_;


        char readBuffer[read_len];
        memset(readBuffer, 0, read_len);
        nread = sock_read_data(fd, readBuffer, read_len);
        // queryBuf_.AppendData(readBuffer,nread);

        RTC_LOG(LS_INFO) << "TcpConnection::read _on_recv_notify fd:" << fd
                         << ", nread:" << nread
                         <<", queryBuf_.size():" <<queryBuf_.size();
        if (-1 == nread)
        {
            // _close_connection(fd);
            return -1;
        }
        else if (nread > 0 )
        {
            try
            {
                queryBuf_.AppendData(readBuffer, nread);
            }
            catch (const std::exception &e)
            {
                RTC_LOG(LS_ERROR) << "queryBuf_.AppendData error";
            }

            if(queryBuf_.size() >= L_HEADER_SIZE)_recv(queryBuf_.data(), nread,callback);
            // delete[] data;
        }

        return 0;
    }
    int TcpConnection::_recv(char *buf, int len, 
                            std::function<void(Json::Value, uint32_t)> callback)
    {
        if (current_state_ == STATE_HEAD)
        {
            _parseDataIntoLHeader(buf, len, req_header_);
            // req_header_
                RTC_LOG(LS_INFO) << "TcpConnection::recv header:" << req_header_.toString();
                return 0;
        }
        if (current_state_ == STATE_BODY)
        {
            std::string body;
           bool retBool =  _parseDataOfBodyJson(buf+L_HEADER_SIZE, len, req_header_.body_len, body);
            if (!retBool)
            {
                RTC_LOG(LS_WARNING) << "parse body json err : " << "log_id:" << req_header_.log_id;
                return -1;
            }
            
            if (body.length() < 0)
            {
                return 0;
            }

            Json::CharReaderBuilder buildr;
            std::unique_ptr<Json::CharReader> reader(buildr.newCharReader());
            Json::Value root;
            JSONCPP_STRING errs;
            reader->parse(body.c_str(), body.c_str() + body.size(), &root, &errs);
            if (!errs.empty())
            {
                RTC_LOG(LS_WARNING) << "parse body json err : " << errs << ",log_id:" << req_header_.log_id;
                return -1;
            }
            RTC_LOG(LS_INFO) << "TcpConnection::recv parse body success!! body:" << body;
            if (callback)
                callback(root, req_header_.log_id);
        }
       
        

        
       
        
       

        //   RTC_LOG(LS_INFO) << "id: " << header.id;
        //     RTC_LOG(LS_INFO) << "version: " << header.version;
        //     RTC_LOG(LS_INFO) << "log_id: " << header.log_id;
        //     RTC_LOG(LS_INFO) << "provider: " << std::string(header.provider, sizeof(header.provider));
        //     RTC_LOG(LS_INFO) << "magic_num: "<< header.magic_num  << ", L_HEADER_MAGIC_NUMBER=" << L_HEADER_MAGIC_NUMBER;;
        //     RTC_LOG(LS_INFO) << "reserved: " << header.reserved;
        //     RTC_LOG(LS_INFO) << "body_len: " <<  header.body_len;
        //     RTC_LOG(LS_INFO) << "body: " << body;
        // char *data = new char[sizeof(lheader_t)];
        // memcpy(data, buf, sizeof(lheader_t));
        // lheader_t *header_copy = reinterpret_cast<lheader_t *>(data);
        // RTC_LOG(LS_INFO) << "id: " << header_copy->id;
        //     RTC_LOG(LS_INFO) << "version: " << header_copy->version;
        //     RTC_LOG(LS_INFO) << "log_id: " << header_copy->log_id;
        //     RTC_LOG(LS_INFO) << "provider: " << std::string(header_copy->provider, sizeof(header_copy->provider));
        //     RTC_LOG(LS_INFO) << "magic_num: "<< header_copy->magic_num  << ", L_HEADER_MAGIC_NUMBER=" << L_HEADER_MAGIC_NUMBER;;
        //     RTC_LOG(LS_INFO) << "reserved: " << header_copy->reserved;
        //     RTC_LOG(LS_INFO) << "body_len: " <<  header_copy->body_len;

        return 0;
    }
    int TcpConnection::close_conn()
    {
        close(fd_);
        return 0;
    }
    void TcpConnection::writerHeaderDataToBuffer(const lheader_t& header, rtc::ByteBufferWriter &writer)
    {
        // 将 id 写入 ByteBufferWriter
        writer.WriteUInt16(header.id);

        // 将 version 写入 ByteBufferWriter
        writer.WriteUInt16(header.version);

        // 将 log_id 写入 ByteBufferWriter
        writer.WriteUInt32(header.log_id);

       writer.WriteBytes(header.provider, sizeof(header.provider));
        // 将 magic_num 写入 ByteBufferWriter
        writer.WriteUInt32(header.magic_num);

        // 将 reserved 写入 ByteBufferWriter
        writer.WriteUInt32(header.reserved);

        // 将 body_len 写入 ByteBufferWriter
        writer.WriteUInt32(header.body_len);
    }
    // Function to parse data into lheader_t structure
    bool TcpConnection::_parseDataIntoLHeader(const char *data, size_t data_size, lheader_t &header)
    {
        if (current_state_!= STATE_HEAD)
        {
            return false;
        }
        
        // Check if data size is at least the size of the header
        if (data_size < sizeof(lheader_t))
        {
            return false; // Data size is too small
        }

        rtc::ByteBufferReader reader(data, data_size, rtc::ByteBuffer::ORDER_HOST);

        // Read each field of the lheader_t structure
        reader.ReadUInt16(&header.id);
        reader.ReadUInt16(&header.version);
        reader.ReadUInt32(&header.log_id);
        reader.ReadBytes(header.provider, sizeof(header.provider));
        reader.ReadUInt32(&header.magic_num);
        reader.ReadUInt32(&header.reserved);
        reader.ReadUInt32(&header.body_len);
        if (L_HEADER_MAGIC_NUMBER != header.magic_num)
        {
            current_state_ = STATE_HEAD;
            bytes_expected_ = L_HEADER_SIZE;
            RTC_LOG(LS_WARNING) << "invalid magic number:" << header.magic_num
                                << ", L_HEADER_MAGIC_NUMBER=" << L_HEADER_MAGIC_NUMBER;
            return false;
        
        }
        current_state_ = STATE_BODY;
        bytes_expected_ = header.body_len;
        
        

        return true; // Parsing successful
    }
    bool TcpConnection::_parseDataOfBodyJson(const char *data, size_t data_size,size_t  body_len,std::string &body)
    {
        if (current_state_ == STATE_BODY)
        {
            if (data_size > 0 && data_size >= body_len)
            {
                rtc::ByteBufferReader reader(data, data_size, rtc::ByteBuffer::ORDER_HOST);

                reader.ReadString(&body, body_len);
                return true;
            }
            else
            {
                RTC_LOG(LS_ERROR) << "lym body_len:" << body_len << ", data_size:" << data_size;
                return false;
            }
        }
        
    }
    // size_t TcpConnection::unpackageHeader(rtc::ArrayView<const uint8_t> package_frame,
    //                                                  uint8_t version[4],
    //                                                  uint8_t *iden,
    //                                                  uint64_t *keyValidityPeriod,
    //                                                  std::string &extIdStrd,
    //                                                  rtc::ArrayView<uint8_t>
    //                                                      unpackage_frame)
    // {
    //     size_t encodedSize = package_frame.size();
    //     size_t destLen = (encodedSize / 4) * 3;
    //     if (destLen == 0)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym encoded Size not 0!!!!";
    //         return 0;
    //     }
    //     //    rtc::BufferT<uint8_t> decodedData(destLen);
    //     //    uint8_t* decodedData = new uint8_t[outSize];

    //     //    size_t decodedSize = base64_decode(package_frame.data(), package_frame.size(), decodedData.data()) - 1;//
    //     rtc::BufferT<uint8_t> decodedData(package_frame.data(), package_frame.size());
    //     size_t decodedSize = encodedSize;
    //     if (decodedSize <= 8)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym encoded Size is lelitt！！！";
    //         return 0;
    //     }
    //     int cipLenT = 0;
    //     //    int offset = 0;

    //     //    memcpy(&(cipStrOffset), decodedData.data(), 2);
    //     rtc::ByteBufferReader bufferReader(reinterpret_cast<const char *>(package_frame.data()), encodedSize);

    //     uint16_t cipStrOffset = 0;
    //     bufferReader.ReadUInt16(&cipStrOffset);
    //     //    //invalid package
    //     if (cipStrOffset >= decodedSize || cipStrOffset <= 8)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym invalid package 1 ！！！";
    //         return 0;
    //     }
    //     cipLenT = decodedSize - cipStrOffset;
    //     if (cipLenT <= 0)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym invalid package 2 ！！！";
    //         return 0;
    //     }
    //     bufferReader.ReadBytes(reinterpret_cast<char *>(version), 4);
    //     bufferReader.ReadUInt8(iden);
    //     ////    if (iden != enCiphertextMode::enCiphertext_bxy && iden != enCiphertextMode::enCiphertext_xtqss && iden != enCiphertextMode::enCiphertext_unkown) {
    //     ////        tools::commLog("invalid enCiphertextMode ");
    //     ////        return false;
    //     ////    }
    //     // keydi的有效期获取
    //     bufferReader.ReadUInt64(keyValidityPeriod);

    //     uint8_t keylenT = 0;
    //     bufferReader.ReadUInt8(&keylenT);
    //     if (keylenT < 0)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym invalid keylen ！！！";
    //         return 0;
    //     }
    //     if (keylenT > decodedSize - 8)
    //     {
    //         RTC_LOG(LS_ERROR) << "lym invalid keylen 2 ！！！";
    //         return 0;
    //     }
    //     if (keylenT != 0)
    //     {
    //         bufferReader.ReadString(&extIdStrd, keylenT);
    //     }
    //     bufferReader.ReadBytes(reinterpret_cast<char *>(unpackage_frame.data()), cipLenT);
    //     return cipLenT;
    // }

} // namespace lrtc
