## MaxMind GeoIP2

This plugin allows you to lookup an IP address with MaxMind's GeoIP2 database. The plugin will return the country name ``country`` (string) and country ISO code ``iso_code`` (string) as an associative array. An empty array will be returned if no data is available for the queried IP. Upon error the function will return an associative array with a ``error`` (string) property. This plugin depends on the libmaxminddb library (libmaxminddb-dev) and the appropiate GeoLite2 databased obtained from MaxMind.

## Configuration example

smtpd.yaml

```
plugins:
  - id: geoip
    path: /opt/halon/plugins/geoip.so
    config:
      path: /path/to/GeoLite2-Country.mmdb
```

## HSL usage

A function called `geoip` is now available in HSL and can be used in any context.

An example how you can use it in the connect hook.

```
$remoteip = $arguments["remoteip"];
echo "GeoIP: ".geoip($remoteip);
```
