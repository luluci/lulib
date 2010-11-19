#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				// io_service��async_read���Z�b�g����
				template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
				void connect(Protocol &p, EndpointIterator &it, Success const& success, Failure const& failure) {
					// �v���g�R�������Ă�����I��
					if (!p) return;

					// endpoint�̉������J�n
					p.socket().async_connect( *it,
						[&](boost::system::error_code const& error) {
							detail::connect_handle(error, p, ++it, success, failure);
						}
					);

				}

				namespace detail {
					template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
					void connect_handle(
						boost::system::error_code const& error,
						Protocol &p,
						EndpointIterator &it,
						Success const& success, Failure const& failure
					) {
						// �v���g�R�������Ă�����I��
						if (!p) return;

						// connect�ɐ�������
						if (!error) {
							//p.async_success();
							success();
						}
						// connect�Ɏ��s����
						// ���Aendpoint_iterator���I�[�ɒB���Ă��Ȃ��Ȃ�A�ċA����
						else if (it != EndpointIterator()) {
							// ���s�����ڑ������
							p.socket().close();
							// ���g���C
							p.socket().async_connect( *it,
								[&](boost::system::error_code const& error) {
									connect_handle(error, p, ++it, success, failure);
								}
							);
						}
						// �I�[�ɒB���Ă�����A���s
						else {
							//p.async_failure();
							failure();
						}
					}
				}//namespace detail

			}//namespace async
		}//namespace protocol
	}//namespace network
}//namespace lulib
