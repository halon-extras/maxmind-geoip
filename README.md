## MaxMind GeoIP2

This plugin allows you to lookup an IP address with [MaxMind's GeoIP2 database](https://dev.maxmind.com/geoip/geolite2-free-geolocation-data). Depending on the database type used the plugin will return different values. The plugin depends on the libmaxminddb library (libmaxminddb-dev) and the appropiate GeoLite2 databases obtained from MaxMind.
Upon error the function will return an associative array with a ``error`` (string) property.

### Country
If a country database is used the plugin will return the country name ``country`` (string) and country ISO code ``iso_code`` (string) as an associative array. An empty array will be returned if no data is available for the queried IP. 

### ASN
If an ASN database is used the plugin will return the organization name ``organization`` (string) and asn ``number`` (number) as an associative array. An empty array will be returned if no data is available for the queried IP. 

## Installation

Follow the [instructions](https://docs.halon.io/manual/comp_install.html#installation) in our manual to add our package repository and then run the below command.

### Ubuntu

```
apt-get install halon-extras-maxmind-geoip
```

### RHEL

```
yum install halon-extras-maxmind-geoip
```

## Configuration example

smtpd.yaml

```
plugins:
  - id: geoip
    config:
      path: /path/to/GeoLite2-Country.mmdb
```

## HSL usage

A function called `geoip` is now available in HSL and can be used in any context.

An example how you can use it in the connect hook.

```
import { geoip } from "extras://geoip";
$remoteip = $arguments["remoteip"];
echo "GeoIP: ".geoip($remoteip);
```
