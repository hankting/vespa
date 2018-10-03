// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.vespa.model.container.http.xml;

import com.yahoo.config.model.builder.xml.XmlHelper;
import com.yahoo.config.model.producer.AbstractConfigProducer;
import com.yahoo.text.XML;
import com.yahoo.vespa.model.builder.xml.dom.VespaDomBuilder;
import com.yahoo.vespa.model.container.component.SimpleComponent;
import com.yahoo.vespa.model.container.http.ConnectorFactory;
import com.yahoo.vespa.model.container.http.ssl.CustomSslProvider;
import com.yahoo.vespa.model.container.http.ssl.DefaultSslProvider;
import com.yahoo.vespa.model.container.http.ssl.LegacySslProvider;
import org.w3c.dom.Element;

import java.util.Optional;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @author Einar M R Rosenvinge
 * @author mortent
 */
public class JettyConnectorBuilder extends VespaDomBuilder.DomConfigProducerBuilder<ConnectorFactory>  {

    @Override
    protected ConnectorFactory doBuild(AbstractConfigProducer ancestor, Element serverSpec) {
        String name = XmlHelper.getIdString(serverSpec);
        int port = HttpBuilder.readPort(serverSpec, ancestor.getRoot().getDeployState());

        SimpleComponent sslProviderComponent = getSslConfigComponents(name, serverSpec);
        return new ConnectorFactory(name, port, sslProviderComponent);
    }

    SimpleComponent getSslConfigComponents(String serverName, Element serverSpec) {
        Element sslConfigurator = XML.getChild(serverSpec, "ssl");
        Element sslProviderConfigurator = XML.getChild(serverSpec, "ssl-provider");

        if (sslConfigurator != null) {
            String privateKeyFile = XML.getValue(XML.getChild(sslConfigurator, "private-key-file"));
            String certificateFile = XML.getValue(XML.getChild(sslConfigurator, "certificate-file"));
            Optional<String> caCertificateFile = XmlHelper.getOptionalChildValue(sslConfigurator, "ca-certificates-file");
            Optional<String> clientAuthentication = XmlHelper.getOptionalChildValue(sslConfigurator, "client-authentication");
            return new DefaultSslProvider(
                    serverName,
                    privateKeyFile,
                    certificateFile,
                    caCertificateFile.orElse(null),
                    clientAuthentication.orElse(null));
        } else if (sslProviderConfigurator != null) {
            String className = sslProviderConfigurator.getAttribute("class");
            String bundle = sslProviderConfigurator.getAttribute("bundle");
            return new CustomSslProvider(serverName, className, bundle);
        } else {
            return new LegacySslProvider(serverName);
        }
    }
}
