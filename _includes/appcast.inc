<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:sparkle="http://www.andymatuschak.org/xml-namespaces/sparkle" xmlns:dc="http://purl.org/dc/elements/1.1/">
    <channel>
        <title>{{ site.github.project_title }}</title>
        <description>Most recent changes with links to updates.</description>
        <language>en</language>
        {% for release in site.github.releases %}
            {% unless release.draft %}
                {% unless release.prerelease and page.release_only %}
                    <item>
                        <title>{{ release.name }}</title>
                        <description><![CDATA[{{ release.body | markdownify }}]]></description>
                        <pubDate>{{ release.published_at | date_to_rfc822 }}</pubDate>
                        {% assign version = release.tag_name | remove_first:'v' %}
                        {% for asset in release.assets  %}
	                        {% if asset.name contains "macOS" %}            
                            <enclosure
                                url="{{ asset.browser_download_url }}"
                                sparkle:version="{{ version }}"
                                sparkle:os="macos"
                                sparkle:shortVersionString="{{ version }}"
                                length="{{ asset.size }}"
                                type="application/octet-stream" />
                          {% endif %}
                          {% if asset.name contains "windows-x86" %}            
                            <enclosure
                                url="{{ asset.browser_download_url }}"
                                sparkle:version="{{ version }}"
                                sparkle:os="windows-x86"
                                sparkle:installerArguments="/SILENT /SP- /NOICONS"
                                sparkle:shortVersionString="{{ version }}"
                                length="{{ asset.size }}"
                                type="application/octet-stream" />
                          {% endif %}   
                          {% if asset.name contains "windows-x64" %}            
                            <enclosure
                                url="{{ asset.browser_download_url }}"
                                sparkle:version="{{ version }}"
                                sparkle:os="windows-x64"
                                sparkle:installerArguments="/SILENT /SP- /NOICONS"
                                sparkle:shortVersionString="{{ version }}"
                                length="{{ asset.size }}"
                                type="application/octet-stream" />
                          {% endif %}  
                        {% endfor %}
                    </item>
                {% endunless %}
            {% endunless %}
        {% endfor %}
    </channel>
</rss>