#include "server/tcp_connection.h"
#include <cstring>
#include <unistd.h> // 包含 close 函数的头文件

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
        int nread = 0;
        const int read_len = L_HEADER_SIZE * 4;

        char readBuffer[read_len];
        memset(readBuffer, 0, read_len);
        nread = sock_read_data(fd, readBuffer, read_len);
        RTC_LOG(LS_INFO) << "TcpConnection::read _on_recv_notify fd:" << fd
                         << " nread:" << nread;
        if (-1 == nread)
        {
            // _close_connection(fd);
            return -1;
        }
        else if (nread > 0)
        {
            // // 创建二进制数据并分配足够的内存
            // char *data = new char[sizeof(lheader_t)];

            // // 填充 lheader_t 的实例 t 的数据
            // lheader_t t;
            // t.id = 1234;
            // t.version = 1;
            // t.log_id = 5678;
            // std::strcpy(t.provider, "Provider"); // 使用 strcpy 将字符串复制到 provider 中
            // t.magic_num = 0xfb202404;
            // t.reserved = 0;
            // t.body_len = 1024;

            // // 使用 memcpy 将 t 的数据复制到 data 中
            // memcpy(data, &t, sizeof(lheader_t));

            _recv(readBuffer, nread,callback);
            // delete[] data;
        }

        return 0;
    }
    int TcpConnection::_recv(char *buf, int len, 
                            std::function<void(Json::Value, uint32_t)> callback)
    {
        lheader_t header;
        std::string body;
        _parseDataIntoLHeader(buf, len, header, body);
         // 现在可以访问结构体的字段了
        RTC_LOG(LS_INFO) << "TcpConnection::recv header:" << header.toString()
                         << ", \nbody:" << body
                         << "\nprovider: " << std::string(header.provider, sizeof(header.provider));
        Json::CharReaderBuilder buildr;
        std::unique_ptr<Json::CharReader> reader(buildr.newCharReader());
        Json::Value root;
        JSONCPP_STRING errs;
        reader->parse(body.c_str(), body.c_str() + body.size(), &root, &errs);
        if(!errs.empty())
        {
            RTC_LOG(LS_WARNING) << "parse body json err : " << errs << ",log_id:" << header.log_id;
            return -1;

        }
        if (callback)callback(root,header.log_id);

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
    // Function to parse data into lheader_t structure
    bool TcpConnection::_parseDataIntoLHeader(const char *data, size_t data_size, lheader_t &header, std::string &body)
    {
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
            RTC_LOG(LS_WARNING) << "invalid magic number:" << header.magic_num
                                << ", L_HEADER_MAGIC_NUMBER=" << L_HEADER_MAGIC_NUMBER;
            return false;
        }

        if (reader.Length() > 0 && reader.Length() >= header.body_len)
        {
            reader.ReadString(&body, header.body_len);
        }
        else
        {
            RTC_LOG(LS_ERROR) << "lym body_len:" << header.body_len << ", reader.Length():" << reader.Length();
        }

        return true; // Parsing successful
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
