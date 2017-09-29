/*
	restinio
*/

/*!
	WebSocket messgage handler definition.
*/

#pragma once

#include <functional>

#include <restinio/utils/impl/bitops.hpp>

namespace restinio
{

namespace websocket
{

#define RESTINIO_WEBSOCKET_OPCODES_MAP( RESTINIO_GEN ) \
	RESTINIO_GEN( continuation_frame,     0x00 ) \
	RESTINIO_GEN( text_frame,             0x01 ) \
	RESTINIO_GEN( binary_frame,           0x02 ) \
	RESTINIO_GEN( connection_close_frame, 0x08 ) \
	RESTINIO_GEN( ping_frame,             0x09 ) \
	RESTINIO_GEN( pong_frame,             0x0A )

//
// opcode_t
//

enum class opcode_t : std::uint8_t
{
#define RESTINIO_WEBSOCKET_OPCODES_GEN( name, code ) name = code,
	RESTINIO_WEBSOCKET_OPCODES_MAP( RESTINIO_WEBSOCKET_OPCODES_GEN )
#undef RESTINIO_WEBSOCKET_OPCODES_GEN
	unknown_frame
};

//! Helper sunction to get method string name.
inline const char *
opcode_to_string( opcode_t opcode )
{
	const char * result = "unknown_frame";
	switch( opcode )
	{
		#define RESTINIO_WEBSOCKET_OPCODES_GEN( name, code ) \
			case opcode_t::name: result = #name; break;

			RESTINIO_WEBSOCKET_OPCODES_MAP( RESTINIO_WEBSOCKET_OPCODES_GEN )
		#undef RESTINIO_WEBSOCKET_OPCODES_GEN

		default:; // Ignore.
	};

	return result;
}

//
//  status_code_t
//

enum class status_code_t : std::uint16_t
{
	normal_closure = 1000,
	going_away = 1001,
	protocol_error = 1002,
	cant_accept_data = 1003,
	no_status_provided = 1005,
	connection_lost = 1006,
	invalid_message_data = 1007,
	policy_violation = 1008,
	too_big_message = 1009,
	more_extensions_expected = 1010,
	unexpected_condition = 1011
};

inline std::string
status_code_to_bin( status_code_t code )
{
	using namespace ::restinio::utils::impl::bitops;

	return {
		n_bits_from<char, 8>( static_cast<std::uint16_t>(code) ),
		n_bits_from<char, 0>( static_cast<std::uint16_t>(code) )
	};
}

inline status_code_t
status_code_from_bin( std::string data )
{
	using namespace ::restinio::utils::impl::bitops;

	std::uint16_t result{ 0 };
	if( 2 >= data.size() )
	{
		result |= static_cast< std::uint8_t >( data[ 0 ] );
		result <<= 8;
		result |= static_cast< std::uint8_t >( data[ 1 ] );
	}

	// TODO: make it ok.
	return (status_code_t)result;
}


//
// message_t
//

//! WebSocket message.
class message_t final
	:	public std::enable_shared_from_this< message_t >
{
	public:

		message_t() = default;

		message_t(
			bool is_final,
			opcode_t opcode )
			:	m_is_final{ is_final }
			,	m_opcode{ opcode }
		{}

		message_t(
			bool is_final,
			opcode_t opcode,
			std::string payload )
			:	m_is_final{ is_final }
			,	m_opcode{ opcode }
			,	m_payload{ std::move( payload ) }
		{}

		bool
		is_final() const
		{
			return m_is_final;
		}

		opcode_t
		opcode() const
		{
			return m_opcode;
		}

		void
		set_opcode( opcode_t opcode )
		{
			m_opcode = opcode;
		}

		const std::string&
		payload() const
		{
			return m_payload;
		}

		std::string&
		payload()
		{
			return m_payload;
		}

		void
		set_payload( std::string str )
		{
			m_payload = std::move( str );
		}

	private:

		//! Final flag.
		bool m_is_final = true;

		//! Opcode.
		opcode_t m_opcode = opcode_t::continuation_frame;

		//! Websocket message payload.
		std::string m_payload;
};

//! Request handler, that is the type for calling request handlers.
using message_handle_t = std::shared_ptr< message_t >;

//
// default_request_handler_t
//

using default_message_handler_t =
		std::function< void ( message_handle_t ) >;


} /* namespace websocket */

} /* namespace restinio */
