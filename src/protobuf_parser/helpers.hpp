#ifndef SRC_PROTOBUF_PARSER_HELPERS_H_
#define SRC_PROTOBUF_PARSER_HELPERS_H_

#include <google/protobuf/message.h>

#include <memory>
#include <vector>

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif
typedef std::vector<char> Data;
typedef std::shared_ptr<const Data> PointerToConstData;
typedef std::shared_ptr<Data> PointerToData;

/*!
 * \brief Сериализует сообщение в поток байт предваряя его длиной
 *
 * \tparam Message Тип сообщения, для работы с которым предназначена данная
 * функция.
 *
 * \param msg Сообщение
 *
 * \return Умный указатель на вектор байт.
 */
template <typename Message> PointerToConstData serializeDelimited(const Message& msg)
{
  const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
  const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

  const PointerToData& result = std::make_shared<Data>(headerSize + messageSize);

  google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());
  google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
  msg.SerializeWithCachedSizesToArray(buffer + headerSize);
  return result;
}

/*!
 * \brief Расшифровывает сообщение, предваренное длиной из массива байтов.
 *
 * \tparam Message Тип сообщения, для работы с которым предназначена данная
 * функция.
 *
 * \param data Указатель на буфер данных.
 * \param size Размер буфера данных.
 * \param bytesConsumed Количество байт, которое потребовалось для расшифровки
 * сообщения в случае успеха.
 *
 * \return Умный указатель на сообщение. Если не удалось расшифровать размер сообщения
 * или если было передано сообщение меньшего размера, то он пустой.
 *
 * \throw std::runtime_error Если расшифровка сообщения не удалась.
 */
template <typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size,
                                        size_t* bytesConsumed = nullptr)
{
  const google::protobuf::uint8* buffer = reinterpret_cast<const google::protobuf::uint8*>(data);
  google::protobuf::io::CodedInputStream codedInput(buffer, size);

  uint32_t messageSize = 0;
  if (!codedInput.ReadVarint32(&messageSize))
  {
    if (bytesConsumed != nullptr)
      *bytesConsumed = 0;
    return nullptr;
  }

  size_t bytesRead = codedInput.CurrentPosition();
  if (size < bytesRead + messageSize)
  {
    if (bytesConsumed != nullptr)
      *bytesConsumed = 0;
    return nullptr;
  }

  codedInput.PushLimit(messageSize);

  std::shared_ptr<Message> message = std::make_shared<Message>();
  if (!message->ParseFromCodedStream(&codedInput))
    throw std::runtime_error("wrong data");

  if (bytesConsumed != nullptr)
    *bytesConsumed = codedInput.CurrentPosition();

  return message;
}


#endif /* SRC_PROTOBUF_PARSER_HELPERS_H_ */
