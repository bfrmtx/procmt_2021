<?php
include "AduDbApi.php";

/**
 * Checks request parameters and delegates requests to the proper calls of AduDbApi.
 * For simplicity's sake only GET and POST requests will be handled.
 * 
 * Conventions:
 * - GET requests will not alter the state of the ADU
 * - POST requests might have side effects that alter the state of the ADU
 * 
 * Intended use:
 * GET http://localhost/ws/AduWebService.php?resource=hwConfig
 * GET http://localhost/ws/AduWebService.php?resource=hwDatabase
 * GET http://localhost/ws/AduWebService.php?resource=hwStatus
 * GET http://localhost/ws/AduWebService.php?resource=channelConfig
 * GET http://localhost/ws/AduWebService.php?resource=selftest
 * GET http://localhost/ws/AduWebService.php?resource=jobs
 * GET http://localhost/ws/AduWebService.php?resource=futureJobs
 * GET http://localhost/ws/AduWebService.php?resource=job&id=1
 *
 * POST http://localhost/ws/AduWebService.php?resource=job
 * POST http://localhost/ws/AduWebService.php?resource=stopJob
 * POST http://localhost/ws/AduWebService.php?resource=clearJobs
 * POST http://localhost/ws/AduWebService.php?resource=channelConfig
 * POST http://localhost/ws/AduWebService.php?resource=startCopyTSToUSB
 */
$api = new AduDbApi('PDO'); //Prepares a new connection

$method = $_SERVER['REQUEST_METHOD'];

$validGetResourceNames = ['hwConfig' 	  => [$api, 'getHwConfig'],
                          'hwDatabase'	  => [$api, 'getHwDatabase'],
                          'hwStatus' 	  => [$api, 'getHwStatus'],
                          'channelConfig' => [$api, 'getChannelConfig'],
                          'selftest' 	  => [$api, 'getSelftest'],
                          'jobcount' 	  => [$api, 'getJobcount'],
                          'jobs' 	      => [$api, 'getJobs'],
                          'joblistnames'  => [$api, 'getJobListNames'],
                          'futureJobs' 	  => [$api, 'getFutureJobs'],
                          'job' 	      => [$api, 'getJob'],
                          'openmtmeasdoc' => [$api, 'getOpenMTMeasDoc']
                         ];
// TUT 8: make sure that your desired function is registered
//  urlParameter => [$object, 'nameOfMethod']
$validPostResourceNames = [ 'job' 		             => [$api, 'postJob'],
                            'startCopyTSToUSB'	     => [$api, 'postStartCopyTSToUSB'],
                            'stopJob' 		         => [$api, 'postStopJob'],
                            'clearJobs' 	         => [$api, 'postClearJobs'],
                            'clearFutureJobs' 	     => [$api, 'postClearFutureJobs'], // TUT 19: the new clear mapping
                            'channelConfig' 	     => [$api, 'postChannelConfig'],
                            'ejectTSDataSDCard'	     => [$api, 'postEjectTSDataSDCard'],
                            'insertTSDataSDCard'     => [$api, 'postInsertTSDataSDCard'],
                            'formatTSDataSDCard'     => [$api, 'postFormatTSDataSDCard'],
                            'startJoblist'           => [$api, 'postStartJoblist'],
                            'toggleGPSDynaMode'      => [$api, 'postToggleGPSDynaMode'],
                            'restartSensorTest'      => [$api, 'postRestartSensorTest'],
                            'requestSensorDetection' => [$api, 'postRequestSensorDetection'],
                          ];
                        

switch ($method) {
  case 'POST':
    parse_str($_SERVER['QUERY_STRING'], $postParams);
    if (!isset($postParams['resource']) || !array_key_exists($postParams['resource'], $validPostResourceNames)) {
        $protocol = (isset($_SERVER['SERVER_PROTOCOL']) ? $_SERVER['SERVER_PROTOCOL'] : 'HTTP/1.0');
        header($protocol . ' ' . 400 . ' ' . 'Bad Request');
        http_response_code(400);
        echo "Your request was malformed.";
    } else {
        call_user_func($validPostResourceNames[$postParams['resource']]);
    }
    break;
  case 'GET':
    if (!isset($_GET['resource']) || !array_key_exists($_GET['resource'], $validGetResourceNames)) {
        $protocol = (isset($_SERVER['SERVER_PROTOCOL']) ? $_SERVER['SERVER_PROTOCOL'] : 'HTTP/1.0');
        header($protocol . ' ' . 400 . ' ' . 'Bad Request');
        http_response_code(400);
        echo "Your request was malformed.";
    } else {
        if ($_GET['resource'] === 'job') { 
            call_user_func_array($validGetResourceNames[$_GET['resource']], [$_GET['id']]);
        } else { // TUT 9: I handle parametrized requests explicitly, but clearJobs is simple, now to AduDbApi::postClearJobs
            call_user_func($validGetResourceNames[$_GET['resource']]);
        }
    }
    break;
  default:
    $protocol = (isset($_SERVER['SERVER_PROTOCOL']) ? $_SERVER['SERVER_PROTOCOL'] : 'HTTP/1.0');
    header($protocol . ' ' . 501 . ' ' . 'Not Implemented');
    http_response_code(501);
    echo "The requested method is not implemented yet";
    break;
    break;
}

$api->closeDB ();
unset ($api)

?>
