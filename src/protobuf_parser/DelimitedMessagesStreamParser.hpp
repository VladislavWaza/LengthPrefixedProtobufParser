#ifndef SRC_PROTOBUF_PARSER_DELIMITEDMESSAGESSTREAMPARSER_HPP_
#define SRC_PROTOBUF_PARSER_DELIMITEDMESSAGESSTREAMPARSER_HPP_

#include "helpers.hpp"

#include <list>
#include <memory>

/*!
 * \brief Парсер сообщений, предвареных длиной.
 *
 * Предназначен для разбора потока сообщений. Если на вход поступает часть сообщения,
 * недостаточная для расшифровки, то она сохраняется в буфере и при поступлении остальной части сообщения
 * оно расшифровывается полностью. 
 *  
 * \tparam MessageType Тип сообщения, для работы с которым предназначен парсер.
 */
template <typename MessageType>
class DelimitedMessagesStreamParser
{
 public:
  typedef std::shared_ptr<const MessageType> PointerToConstValue;

  /*!
   * \brief Расшифровывает сообщение, предваренное длиной.
   *
   * \param data Входной поток в виде строки, из которой нужно извлечь сообщения.
   *
   * \return Список умных указателей std::shared_ptr на сообщения. Если не удалось расшифровать 
   * ни одного полного сообщения, будет возвращен пустой список. Дальнейшие вызовы с остальной частью потока
   * приведут к полной расшифровке сообщения.
   *
   * \throw std::runtime_error Если расшифровка сообщения не удалась по причине, не связанной с длинной потока.
   */  
  std::list<PointerToConstValue> parse(const std::string& data);

 private:
  std::vector<char> m_buffer;
};

template <typename MessageType>
std::list<std::shared_ptr<const MessageType>> DelimitedMessagesStreamParser<MessageType>::parse(const std::string &data)
{
  m_buffer.insert(m_buffer.end(), data.begin(), data.end());

  std::list<std::shared_ptr<const MessageType>> result;
  size_t bytesParsed = 0;
  std::shared_ptr<MessageType> delimited = nullptr;
  do
  {
    size_t bytesConsumed = 0;
    delimited = parseDelimited<MessageType>(
                  m_buffer.data() + bytesParsed,
                  m_buffer.size() - bytesParsed,
                  &bytesConsumed
                );
    if (delimited != nullptr)
    {
      result.push_back(delimited);
      bytesParsed += bytesConsumed;
    }
  } while (delimited != nullptr);

  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesParsed);
  return result;
}


#endif /* SRC_PROTOBUF_PARSER_DELIMITEDMESSAGESSTREAMPARSER_HPP_ */