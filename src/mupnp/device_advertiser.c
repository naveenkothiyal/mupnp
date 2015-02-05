/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cdevice_advertiser.c
*
*	Revision:
*
*	06/14/05
*		- first revision
*
*	10/31/05
*		- Added comments
*
*	03/20/06 Theo Beisch
*		- added NULL check on advertiser_stop
*
******************************************************************/

#include <mupnp/device.h>
#include <mupnp/util/time.h>
#include <mupnp/util/log.h>

/**
 * mupnp_upnp_device_advertiser_action
 * 
 * Function run as a UPnP device advertisement
 * thread.
 *
 * \param thread The thread context that this function is running in
 */
static void mupnp_upnp_device_advertiser_action(mUpnpThread *thread) 
{
	mUpnpUpnpDevice *dev;
	mUpnpTime leaseTime;
	mUpnpTime notifyInterval;
  
	mupnp_log_debug_l4("Entering...\n");

  /**
   * Get the device struct that this function is advertising.
   */
  dev = (mUpnpUpnpDevice *)mupnp_thread_getuserdata(thread);

  /**
   * Lease time
   */
  leaseTime = mupnp_upnp_device_getleasetime(dev);
  
  /* Run this thread until it is stopped from outside */
    while ( mupnp_thread_isrunnable(thread) == TRUE )
    {
      notifyInterval = (leaseTime/4) + (long)((float)leaseTime * (mupnp_random() * 0.25f));
      notifyInterval *= 1000;

      /* Sleep *notifyInterval* msecs */
      mupnp_wait(notifyInterval); 

      /* Check if we must exit before sending new announce */
      if ( mupnp_thread_isrunnable(thread) == FALSE ) break;

      /* Initiate advertise routine after sleeping */
      mupnp_upnp_device_announce(dev);
    }

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * mupnp_upnp_device_advertiser_start
 *
 * Start the advertiser thread for the given device
 *
 * \param dev The device that is to be advertised
 */
BOOL mupnp_upnp_device_advertiser_start(mUpnpUpnpDevice *dev)
{
	mUpnpThread *advertiser;
  
	mupnp_log_debug_l4("Entering...\n");

  /* Get the already existing (created in device_new) advertiser thread */
  advertiser = mupnp_upnp_device_getadvertiserthead(dev);

  /* Store the device into the advertiser thread struct's user data */
  mupnp_thread_setuserdata(advertiser, dev);

  /* Set the function that will run the thread */
  mupnp_thread_setaction(advertiser, mupnp_upnp_device_advertiser_action);

  /* Start the thread */
  return mupnp_thread_start(advertiser);

	mupnp_log_debug_l4("Leaving...\n");
}

/**
 * mupnp_upnp_device_advertiser_stop
 *
 * Stop the advertiser thread for the given device
 *
 * \param dev Device that is being advertised
 */
BOOL mupnp_upnp_device_advertiser_stop(mUpnpUpnpDevice *dev)
{
	mUpnpThread *advertiser;
  
	mupnp_log_debug_l4("Entering...\n");

	/* Get the advertiser thread from the device struct */
	advertiser = mupnp_upnp_device_getadvertiserthead(dev);
  
	//Theo Beisch 
	if (advertiser != NULL)
 		return mupnp_thread_stop(advertiser);
	return FALSE;
}

/**
 * mupnp_upnp_device_advertiser_isrunning
 *
 * Check if the advertiser has been started
 *
 * \param dev Device that is being advertised
 */
BOOL mupnp_upnp_device_advertiser_isrunning(mUpnpUpnpDevice *dev)
{
	mUpnpThread *advertiser;
	
	mupnp_log_debug_l4("Entering...\n");
	
	advertiser = mupnp_upnp_device_getadvertiserthead(dev);
	if (advertiser != NULL)
 		return mupnp_thread_isrunnable(advertiser);

	return FALSE;
}