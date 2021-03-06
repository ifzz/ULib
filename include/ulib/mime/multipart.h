// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    multipart.h
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#ifndef U_MULTIPART_H
#define U_MULTIPART_H 1

#include <ulib/base/base.h>
#include <ulib/internal/chttp.h>
#include <ulib/container/vector.h>

/**
 * UMimeMultipartMsg -- class to build a MIME multipart
 *
 * A MIME-formatted message contains one or several MIME sections. MIME headers specify how multiple MIME sections
 * are to be interpreted as a whole (whether they are attached together; whether they are alternative representations
 * of the same content; or something even more esoteric). This manual page gives a very brief, terse, overview of basic
 * MIME concepts. See RFC 2045, RFC 2046, RFC 2047, RFC 2048, and RFC 2049 for a formal definition of MIME-formatted messages.
 * Each string in a MIME message is encoded as a single MIME section. A MIME section consists of at least one header line,
 * "Content-Type:". The "Content-Type:" header gives the type of the data ontained in the string. Other header lines may
 * also be present. Their relative order does not matter. MIME headers are followed by a blank line, then the contents of
 * the string, encoded appropriately. All MIME sections generated by UMimeMultipartMsg will always contain another header,
 * "Content-Transfer-Encoding:". This header gives the encoding method used for the string; it is an optional header,
 * but UMimeMultipartMsg always creates it. The MIME encoding method defaults to "7bit" if this header is absent. 7bit
 * encoding is only suitable for plain text messages in the US-ASCII character set. The "8bit" encoding method is used
 * by plain text messages in other character sets that use octets with the high bit set. An alternative to 8bit encoding
 * is "quoted-printable". The "base64" encoding method is used for string containing binary data (anything other than plain
 * text). MIME sections that contain text messages have their "Content-Type:" header set to "text/plain"; or "text/html"
 * for HTML messages. There are also several other, rare, content types that can be used. MIME sections that contain other
 * kinds of data will use some other, appropriate "Content-Type:" header, such as "image/gif", or "audio/x-wav". MIME
 * sections that contain textual content may also use the base64 encoding method, they are not required to use 7bit, 8bit,
 * or quoted-printable. "text/pdf" sections, that contain PDF files, typically contain binary data and must use the base64
 * encoding. Consequently, MIME sections that typically contain binary data, such as image/gif and audio/x-wav, are free
 * to use encodings other than base64, as long as all the data can be represented by printable characters (but, in practice,
 * that never happens). MIME sections may also contain other, optional, headers such as "Content-Disposition:", "Content-ID:",
 * and "Content-Name:". Consult the appropriate RFCs for the specific usage of these headers. These headers can be added
 * by UMimeMultipartMsg by using the option parameter, as described below. These headers play no part in creating the overall
 * structure of a MIME-encoded message, and UMimeMultipartMsg does not care much about these headers. It simply includes them,
 * and their content, upon request. Multiple string are formatted as a single message MIME message in two steps: first, by
 * creating a MIME section for each string; and then creating a single MIME section that contains other MIME sections.
 * A "multipart/mixed" MIME section contains a collection of MIME sections that represent different objects, attached
 * together. A "multipart/alternative" MIME section contains a collection of MIME sections which are alternative
 * representations of the same object, such as an HTML and a plain text version of the same message. Other "multipart"
 * MIME sections also exist, and their usage is defined by their respective RFCs
 */

class U_EXPORT UMimeMultipartMsg {
public:
   // Check for memory error
   U_MEMORY_TEST

   // Allocator e Deallocator
   U_MEMORY_ALLOCATOR
   U_MEMORY_DEALLOCATOR

   enum Encoding {
      NONE              = -1,
      AUTO              =  0,
      BIT7              =  1,
      BIT8              =  2,
      QUOTED_PRINTABLE  =  3,
      BASE64            =  4
   };

   // --------------------------------------------------------------------------------------------------
   // Creating a multipart MIME collection
   // --------------------------------------------------------------------------------------------------
   // type must be either "mixed", "alternative", or some other MIME multipart content type
   // Additionally, "encoding" can only be "7bit" or "8bit", and will default to "8bit" if not specified
   // --------------------------------------------------------------------------------------------------

   UMimeMultipartMsg(const char* type     =                 "mixed",
                     uint32_t type_len    = U_CONSTANT_SIZE("mixed"),
                     Encoding encoding    = BIT8,
                     const char* header   =                 "MIME-Version: 1.0",
                     uint32_t header_len  = U_CONSTANT_SIZE("MIME-Version: 1.0"),
                     bool bRFC2045MIMEMSG = true);

   ~UMimeMultipartMsg()
      {
      U_TRACE_UNREGISTER_OBJECT(0, UMimeMultipartMsg)
      }

   uint32_t message(UString& body, bool bterminator = true);

   // manage parts

   void add(const UString& _section)
      {
      U_TRACE(0, "UMimeMultipartMsg::add(%V)", _section.rep)

      U_INTERNAL_ASSERT_MAJOR(U_line_terminator_len, 0)
      U_ASSERT_EQUALS(_section.find(boundary, U_line_terminator_len, boundary_len), U_NOT_FOUND)

      vec_part.push(_section);
      }

   uint32_t getNumPart() const            { return vec_part.size(); }

   UString operator[](uint32_t pos) const { return vec_part.at(pos); }

   // -----------------------------------------------------------------------------------------------------
   // Creating a single MIME section
   // -----------------------------------------------------------------------------------------------------
   // The type option encodes content appropriately, adds the "Content-Type: type" and
   // "Content-Transfer-Encoding:" and MIME headers. type can be any valid MIME type, except for multipart
   // The encoding option should be specified. It's more efficient to do so
   // The charset option sets the MIME charset attribute for text/plain content
   // The name option sets the name attribute for Content-Type:
   // Additional headers are specified by the header option, doesn't do anything with them except to insert
   // the headers into the generated MIME section
   // -----------------------------------------------------------------------------------------------------

   static UString section(const UString& content,
                          const char* type = "", uint32_t type_len = 0,
                          Encoding encoding = AUTO, const char* charset = "", const char* name = "",
                          const char* header = "", uint32_t header_len = 0);

   // STREAM

#ifdef U_STDCPP_ENABLE
   friend U_EXPORT ostream& operator<<(ostream& os, UMimeMultipartMsg& m);

   // DEBUG

# ifdef DEBUG
   const char* dump(bool reset) const;
# endif
#endif

protected:
   char boundary[64];
   uint32_t boundary_len;
   UVector<UString> vec_part;

   static const char* str_encoding[4];

private:
   static inline int encodeAutodetect(const UString& content, const char* charset) U_NO_EXPORT __pure;

   U_DISALLOW_COPY_AND_ASSIGN(UMimeMultipartMsg)
};

#endif
