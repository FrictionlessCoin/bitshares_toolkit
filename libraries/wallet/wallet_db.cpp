#include <bts/wallet/wallet_db.hpp>
#include <fc/crypto/aes.hpp>

#include <fc/log/logger.hpp>

namespace bts{ namespace wallet {

  const uint32_t master_key_record::type           =   master_key_record_type; 
  const uint32_t private_key_record::type          =   private_key_record_type; 
                                                       
  const uint32_t wallet_name_record::type          =   name_record_type; 
  const uint32_t wallet_asset_record::type         =   asset_record_type; 
  const uint32_t wallet_account_record::type       =   account_record_type; 
  const uint32_t wallet_transaction_record::type   =   transaction_record_type; 
  const uint32_t wallet_contact_record::type       =   contact_record_type; 
  const uint32_t wallet_meta_record::type          =   meta_record_type; 

  hkey_index wallet_contact_record::get_next_receive_key_index( uint32_t trx_num )
  {
      auto trx_itr = last_recv_hkey_index.find( trx_num );
      if( trx_itr == last_recv_hkey_index.end() )
      {
         last_recv_hkey_index[trx_num] = 0;
         return hkey_index( index, trx_num, 0 );
      }
      else
      {
         return hkey_index( index, trx_num, ++trx_itr->second );
      }
  }

  hkey_index wallet_contact_record::get_next_send_key_index( uint32_t trx_num )
  {
      auto trx_itr = last_send_hkey_index.find( trx_num );
      if( trx_itr == last_send_hkey_index.end() )
      {
         last_send_hkey_index[trx_num] = 0;
         return hkey_index( index, trx_num, 0 );
      }
      else
      {
         trx_itr->second++;
         return hkey_index( index, trx_num, trx_itr->second );
      }
  }

  extended_private_key    master_key_record::get_extended_private_key( const fc::sha512& password )const
  {
     return fc::raw::unpack<extended_private_key>( fc::aes_decrypt( password, encrypted_key ) );
  }

  fc::ecc::private_key    private_key_record::get_private_key( const fc::sha512& password )const
  {
     return fc::raw::unpack<fc::ecc::private_key>( fc::aes_decrypt( password, encrypted_key ) );
  }
  
  private_key_record::private_key_record( int32_t index, int32_t contact_idx, 
                                          const fc::ecc::private_key& key, const fc::sha512& password )
  {
     encrypted_key = fc::aes_encrypt( password, fc::raw::pack( key ) );
  }

} } // bts::wallet