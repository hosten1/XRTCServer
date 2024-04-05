#include "server/tcp_connection.h"
#include <cstring>

#include "base/socket.h"
#include "tcp_connection.h"
#include "rtc_base/logging.h"

namespace lrtc
{
    TcpConnection::TcpConnection(int fd, const char *ip, int port) : 
                                fd_(fd), port_(port)/*,queryBuf_(sdsempty)*/
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
    }

    int TcpConnection::read(int fd)
    {
         //本次实际上读取的大小
        int nread = 0;
        const int read_len = bytes_expected_;
        
        // int qb_len = sdslen(c->queryBuf_);
        // conn->queryBuf_ = sdsMakeRoomFor(conn->queryBuf_,read_len);
        // rtc::BufferT<char> readBuffer(read_len);
        char readBuffer[read_len];
        memset(readBuffer,0,read_len);
        nread = sock_read_data(fd,readBuffer,read_len);
        RTC_LOG(LS_INFO) << "TcpConnection::read _on_recv_notify fd:" << fd
                          << " nread:" << nread;
        if(-1 == nread)
        {
            // _close_connection(fd);
            return -1;
        }else if (nread > 0)
        {
            recv(readBuffer,nread);
            // sdsIncrLen(conn->queryBuf_,nread);
        }

        return 0;
    }
    int TcpConnection::recv(char *buf, int len)
    {
        lheader_t header;
        _parseDataIntoLHeader(buf,len,header);
        RTC_LOG(LS_INFO) << "TcpConnection::recv header:" << header.toString();
        return 0;
    }
    // Function to parse data into lheader_t structure
bool TcpConnection::_parseDataIntoLHeader(const char* data,size_t data_size, lheader_t& header)
{
    // Check if data size is at least the size of the header
    if (data_size < sizeof(lheader_t))
    {
        return false; // Data size is too small
    }

    rtc::ByteBufferReader reader(data,data_size);

    // Read each field of the lheader_t structure
    reader.ReadUInt16(&header.id);
    reader.ReadUInt16(&header.version);
    reader.ReadUInt32(&header.log_id);
    reader.ReadBytes(header.provider, sizeof(header.provider));
    reader.ReadUInt32(&header.magic_num);
    reader.ReadUInt32(&header.reserved);
    reader.ReadUInt32(&header.body_len);

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
